#include "ClientSocket.h"
#include "ChatMessage.h"
#include <iostream>
#include <json.hpp>
#include "ChatApplication.h"
#include <mine/mine.h>
#include <asio/ssl.hpp> // Include ASIO SSL
#include <websocketpp/config/asio_client.hpp>
#include <Windows.h>
#include <filesystem>


#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>

using namespace std;
using Json = nlohmann::json;

/// Handlers
void ClientSocket::OnMessage(websocketpp::connection_hdl hdl, websocketpp::config::asio_client::message_type::ptr msg) {
    std::cout << "Received message: " << msg->get_payload() << std::endl;
    ParseMessage(msg->get_payload());
}

void ClientSocket::OpenLinkInBrowser(std::string url){

    // append ? at end of URL
    // Check if the URL already has a question mark
    if (url.find('?') == std::string::npos) {
        url += "?";  // Append ? only if not present
    };


    // This is platform-specific. Adjust based on your OS.
    #ifdef _WIN32
        ShellExecuteA(0, 0, url.c_str(), 0, 0, SW_SHOW);
    #elif __APPLE__
        std::string command = "open " + url;
        system(command.c_str());
    #else
        std::string command = "xdg-open " + url;
        system(command.c_str());
    #endif

    std::cout << "Opening URL: " << url << "\n";
}

void ClientSocket::SelectFile(){

    /* Open File Dialogue --------------------------------------------------------- */

    OPENFILENAME ofn;       // common dialog box structure
    char szFile[260];      // buffer for file name

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "All Files (*.*)\0*.*\0Text Documents (*.txt)\0*.txt\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // Display the dialog box
    if (GetOpenFileName(&ofn)) {
        std::string selectedFilePath = ofn.lpstrFile;
        // Handle the selected file path (e.g., print it)
        printf("Selected file: %s\n", selectedFilePath.c_str());

        if (UploadFileToServer(selectedFilePath)) {
            std::cout << "File upload successful!" << std::endl;
        } else {
            std::cout << "File upload failed." << std::endl;
        }
    }
}

bool ClientSocket::UploadFileToServer(const std::string& filepath){

    std::string url = "https://" + connectedAddress + ":443";

    std::cout << url << "\n";

    httplib::Client cli(url.c_str());

    // Disable SSL verification
    cli.set_follow_location(true);
    cli.enable_server_certificate_verification(false);

    // Open the file as binary
    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        std::cerr << "Could not open file: " << filepath << std::endl;
        return false;
    }

    // Read the file into a string
    std::string file_data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    // Prepare the multipart form
    httplib::MultipartFormDataItems items;

    httplib::MultipartFormData data;
    data.name = "file";

    // Extract the filename from the selected file path
    std::filesystem::path filePath(filepath);
    std::string filename = filePath.filename().string();

    data.content = file_data;
    data.filename = filename;
    data.content_type = "application/octet-stream";

    items.push_back(data);

    // Perform the upload
    auto res = cli.Post("/api/upload", items);
    if (res) {
        std::cout << "Response code: " << res->status << std::endl;

        std::cout << res->body << "\n";
        Json json = Json::parse(res->body);

        // send to recipiant
        SendChatMessage(json["response"]["body"]["file_url"], client->GetActiveUsers()[chatApplication->GetSelectedUser()].publicKey);

        // send to self
        ChatMessage messageToSelf;
        messageToSelf.date = chatApplication->GetCurrentDateTime(false);
        messageToSelf.sentBy = "me (" + chatApplication->GetPsuedoNameExtractedFromKey(publicKey) + ")";
        messageToSelf.filename = filename;
        messageToSelf.fileURL = json["response"]["body"]["file_url"];
        messageToSelf.isFile = true;
        client->PushMessage(messageToSelf, client->GetActiveUsers()[chatApplication->GetSelectedUser()].publicKey);


        return res->status == 200; // Assuming 200 is the success status code
    } else {
        std::cerr << "Error: " << res.error() << std::endl;
        return false;
    }

    return true;
}


// Define a close handler function
void ClientSocket::OnClose(websocketpp::connection_hdl hdl) {
    std::cout << "Connection closed" << std::endl;

    chatApplication->SetConnectedState(CS_DISCONNECTED);
}

void ClientSocket::OnOpen(websocketpp::connection_hdl hdl) {

    global_hdl = hdl;

    std::cout << "Connection opened!" << std::endl;

    this->counter = 0;

    // Example: Send a message to the server right after the connection opens
    websocketpp::lib::error_code ec;

    // Creating json 
    Json helloMessage;

    srand(time(NULL));

    if (!encryptor.GenerateRSAKeyPair(publicKey, privateKey)) {
        std::cerr << "Failed to generate RSA key pair." << std::endl;
        return;
    }

    std::vector<unsigned char> fingerprintVector;
    if (!encryptor.CreateFingerprint(publicKey, fingerprintVector)) {
        std::cerr << "Fingerprint failed." << std::endl;
        return;
    }

    fingerprint = mine::Base64::encode(encryptor.VectorToString(fingerprintVector));

    helloMessage["type"] = "signed_data";
    helloMessage["data"]["type"] = "hello";
    helloMessage["data"]["public_key"] = publicKey;
    helloMessage["counter"] = counter;
    counter++;

    asioClient.send(global_hdl, to_string(helloMessage), websocketpp::frame::opcode::text);

    // ask for client list 
    Json clientListMessage; 
    clientListMessage["type"] = "client_list_request";

    asioClient.send(global_hdl, to_string(clientListMessage), websocketpp::frame::opcode::text);

    // set our own psuedo name
    chatApplication->SetCurrentPseudoName(ChatApplication::GetPsuedoNameExtractedFromKey(publicKey));
    chatApplication->SetConnectedState(CS_CONNECTED);
}

/// ClientSocket functions
void ClientSocket::Start(std::string address, std::string port) {

    std::string finalAddress = address + ":" + port;
    connectedAddress = address;
    try {

        chatApplication->SetConnectedState(CS_IN_PROGRESS);

        // Set logging to be pretty verbose (everything except message payloads)
        asioClient.set_access_channels(websocketpp::log::alevel::all);
        asioClient.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Initialize the Asio transport policy
        asioClient.init_asio();

        // Set up the TLS context to skip verification
        asioClient.set_tls_init_handler([](websocketpp::connection_hdl) {
            auto ctx = websocketpp::lib::make_shared<asio::ssl::context>(asio::ssl::context::sslv23);
            ctx->set_options(asio::ssl::context::verify_none); // Disable verification
            return ctx;
        });



        asioClient.set_message_handler(bind(&ClientSocket::OnMessage, this, placeholders::_1, placeholders::_2));
        asioClient.set_open_handler(bind(&ClientSocket::OnOpen, this, placeholders::_1));
        asioClient.set_close_handler(bind(&ClientSocket::OnClose, this, placeholders::_1));


        // Create a connection to the server
        websocketpp::lib::error_code ec;
        AsioClient::connection_ptr con = asioClient.get_connection("wss://" + finalAddress, ec); // NOT ENCRYPTED

        if (ec) {
            std::cout << "Could not create connection: " << ec.message() << std::endl;
            return;
        }

        // Connect the connection
        asioClient.connect(con);

        // Start the ASIO io_service run loop
        asioClient.run();



    } catch (websocketpp::exception const & e) {
        std::cout << e.what() << std::endl;
    } catch (...) {
        std::cout << "Unknown exception" << std::endl;
    }    
}


void ClientSocket::End() {
    asioClient.close(global_hdl, websocketpp::close::status::normal, "Client closing connection");
}

int ClientSocket::SendChatMessage(string chatMessage, string currentPublicKey) {

    // Creating json
    Json jsonMessage;

    /*
        type
        data
        counter
        signature - base64 (signature of (data+counter))
    */

    if(currentPublicKey == "Public Chat"){
        // PUBLIC CHAT MESSAGE

        std::vector<unsigned char> recipientFingerprint;
        if (!encryptor.CreateFingerprint(publicKey, recipientFingerprint)) {
            std::cerr << "Fingerprint failed." << std::endl;
            return 1;
        }
        string encodedRecipientFingerprint = mine::Base64::encode(encryptor.VectorToString(recipientFingerprint));

        jsonMessage["type"] = "signed_data";
        jsonMessage["data"]["type"] = "public_chat";
        jsonMessage["data"]["sender"] = encodedRecipientFingerprint;
        jsonMessage["data"]["message"] = chatMessage;
        jsonMessage["counter"] = counter++;

        asioClient.send(global_hdl, to_string(jsonMessage), websocketpp::frame::opcode::text);

        std::cout << "message sent\n";

        return 1;
    }

    std::vector<unsigned char> key(32);
    if (!RAND_bytes(key.data(), key.size())) {
        std::cerr << "Error generating key." << std::endl;
        return 1;
    }

    std::vector<unsigned char> plaintext = encryptor.StringToVector(chatMessage);

    // Containers for ciphertext, IV, and tag
    std::vector<unsigned char> ciphertext, iv, tag;

    // Encrypt the message
    if (!encryptor.AESEncrypt(plaintext, key, ciphertext, iv, tag)) {
        std::cerr << "AES Encryption failed." << std::endl;

        return 1;
    }

    string encodedIV = mine::Base64::encode(encryptor.VectorToString(iv));

    string cipherTag = encryptor.VectorToString(ciphertext);
    cipherTag += encryptor.VectorToString(tag);

    string encodedCiphertext = mine::Base64::encode(cipherTag);

    std::vector<unsigned char> RSAEncryptedAESKey;

    if (!encryptor.RSAEncrypt(key, currentPublicKey, RSAEncryptedAESKey)) {
        std::cerr << "RSA Encryption failed." << std::endl;
        return 1;
    }

    string encodedSymmKey = mine::Base64::encode(encryptor.VectorToString(RSAEncryptedAESKey));


    // NEED FINGERPRINT OF BOTH THE SENDER AND RECIPIENT
    std::vector<unsigned char> recipientFingerprint;
    if (!encryptor.CreateFingerprint(publicKey, recipientFingerprint)) {
        std::cerr << "Fingerprint failed." << std::endl;
        return 1;
    }
    string encodedRecipientFingerprint = mine::Base64::encode(encryptor.VectorToString(recipientFingerprint));

    jsonMessage["type"] = "signed_data";
    jsonMessage["counter"] = counter; // TEMP
    jsonMessage["signature"] = "NEED TO DO"; // TEMP

    jsonMessage["data"]["type"] = "chat";
    jsonMessage["data"]["destination_servers"] = uniqueServerList; //e.g. {"127.0.0.1:1", "127.0.0.1:2", "127.0.0.1:3"};
    jsonMessage["data"]["iv"] = encodedIV;

    jsonMessage["data"]["symm_keys"] = encodedSymmKey;

    // Chat is base64 encoded, AES encrypted
    jsonMessage["data"]["chat"]["message"] = encodedCiphertext;

    try {
        jsonMessage["data"]["chat"]["participants"] = nlohmann::json::array({fingerprint, encodedRecipientFingerprint});
    } catch (const std::exception& e) {
        cerr << "Setting participants failed: " << e.what() << endl;
        return 1;
    }

    asioClient.send(global_hdl, to_string(jsonMessage), websocketpp::frame::opcode::text);

    std::cout << "message sent\n";

    return 0;
}

void ClientSocket::ParseMessage(const std::string& data){

    Encryption encryptor;
    Json json = Json::parse(data);

    string topmostType = json["type"];

    if(topmostType == "signed_data"){
        // handle data messages

         if(json["data"]["type"] == "public_chat"){
            string sender = json["data"]["sender"];

            if(sender == fingerprint){
                return;
            }

            ChatMessage chatMessage({json["data"]["message"], client->GetPseudoNameFromFingerprint(sender), chatApplication->GetCurrentDateTime(false)});

            // handling file download URLS

            //e.g.      https://127.0.0.1:443/download/FILE.EXT

            // is the message prefixed with https?

            std::cout << chatMessage.message.substr(0, 8) << "\n";

            if(chatMessage.message.substr(0, 8) == "https://"){

                int endSectionIndex = chatMessage.message.find_last_of('/');
                int downloadSectionIndex = chatMessage.message.substr(0, endSectionIndex).find_last_of('/');

                std::cout << chatMessage.message.substr(downloadSectionIndex, endSectionIndex - downloadSectionIndex) << "\n";

                if(chatMessage.message.substr(downloadSectionIndex, endSectionIndex - downloadSectionIndex) == "/download"){
                    
                    chatMessage.filename = chatMessage.message.substr(endSectionIndex + 1, chatMessage.message.length());
                    chatMessage.isFile = true;
                    chatMessage.fileURL = chatMessage.message;
                }
            }

            client->PushMessage(chatMessage, "Public Chat");
            return;
        } 

        bool shouldDecode = false;
        auto participants = json["data"]["chat"]["participants"];
        for(size_t i = 1; i < participants.size(); i++){
            try{
                if(participants[i].get<std::string>() == fingerprint){
                    shouldDecode = true;
                }
            } catch (const std::exception& e) {
                cerr << "Fingerprint check failed: " << e.what() << endl;
                return;
            }
        }

        // MIGHT NEED TO BE !shouldDecode !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // CHANGE???
        // if(shouldDecode){
        //     cout << "Should not decode" << endl;
        //     return;
        // }

        string decodedCiphertext, decodedSymmKey, decodedIV;
        try {
            decodedCiphertext = mine::Base64::decode(json["data"]["chat"]["message"]);
            decodedSymmKey = mine::Base64::decode(json["data"]["symm_keys"]);
            decodedIV = mine::Base64::decode(json["data"]["iv"]);
        } catch (const std::exception& e) {
            cerr << "Base64 decoding failed: " << e.what() << endl;
            return;
        }

        std::vector<unsigned char> decryptedSymmKey;
        if (!encryptor.RSADecrypt(encryptor.StringToVector(decodedSymmKey), privateKey, decryptedSymmKey)) {
            std::cerr << "RSA Decryption failed." << std::endl;
            return;
        }

        string AESTag;
        string ciphertext;
        if (decodedCiphertext.size() >= 16) {
            // Get the last 16 bytes
            AESTag = decodedCiphertext.substr(decodedCiphertext.size() - 16);
            ciphertext = decodedCiphertext.substr(0, decodedCiphertext.size() - 16);
        } else {
            std::cerr << "Ciphertext not long enough" << std::endl;
            return;
        }


        std::vector<unsigned char> decryptedMessage;
        if (!encryptor.AESDecrypt(encryptor.StringToVector(ciphertext), decryptedSymmKey, encryptor.StringToVector(decodedIV), encryptor.StringToVector(AESTag), decryptedMessage)) {
            std::cerr << "AES Decryption failed." << std::endl;
            return;
        }


        ChatMessage chatMessage({encryptor.VectorToString(decryptedMessage), client->GetPseudoNameFromFingerprint(participants[0].get<std::string>()), chatApplication->GetCurrentDateTime(false)});


        // handling file download URLS

        //e.g.      https://127.0.0.1:443/download/FILE.EXT

        // is the message prefixed with https?

        std::cout << chatMessage.message.substr(0, 8) << "\n";

        if(chatMessage.message.substr(0, 8) == "https://"){

            int endSectionIndex = chatMessage.message.find_last_of('/');
            int downloadSectionIndex = chatMessage.message.substr(0, endSectionIndex).find_last_of('/');

            std::cout << chatMessage.message.substr(downloadSectionIndex, endSectionIndex - downloadSectionIndex) << "\n";

            if(chatMessage.message.substr(downloadSectionIndex, endSectionIndex - downloadSectionIndex) == "/download"){

                chatMessage.filename = chatMessage.message.substr(endSectionIndex + 1, chatMessage.message.length());
                chatMessage.isFile = true;
                chatMessage.fileURL = chatMessage.message;
            }
        }



        client->PushMessage(chatMessage, client->GetKeyFromFingerprint(participants[0].get<std::string>()));
    }

    if(topmostType == "client_list"){  

        // TEMP : Clears the whole list of clients, should instead determine which clients should be updated and only update those

        const std::vector<ActiveUsers>& activeUsers = client->GetActiveUsers();


        // mark all the active users
        for(int i = 0; i < activeUsers.size(); i++){
            client->MarkClient(i, true);
        }


        // update and unmark clients that we find

        for(auto serverInput : json["servers"]){

            for(auto clientInput : serverInput["clients"]){

                int index = client->GetClientIndex(clientInput);

                if(index != -1){

                    client->MarkClient(index, false);
                } else{
                    client->PushActiveUser(clientInput, serverInput["address"], false);
                }
            }
        }

        // remove self from active users
        client->MarkClient(client->GetClientIndex(publicKey), true);

        client->MarkClient(0, false);

        // remove all the clients which are still marked
        client->RemoveMarkedClients();


        uniqueServerList.clear();

        for(int i = 0; i < activeUsers.size(); i++){

            // add if not found
            if(std::find(uniqueServerList.begin(), uniqueServerList.end(), activeUsers[i].serverOfOrigin) == uniqueServerList.end()){
                uniqueServerList.push_back(activeUsers[i].serverOfOrigin);
            }
        }

    }


}
