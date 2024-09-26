#include "ClientSocket.h"
#include "ChatMessage.h"
#include <iostream>
#include <json.hpp>
#include "ChatApplication.h"
#include "Encryption.h"
#include <mine/mine.h>
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

void ClientSocket::ParseMessage(const std::string& data){

    Json json = Json::parse(data);
    
    string topmostType = json["type"];


    if(topmostType == "signed_data"){

        // handle data messages

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
        client->MarkClient(client->GetClientIndex(to_string(publicKey)), true);

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

void ClientSocket::OpenLinkInBrowser(const std::string& url){
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

    std::cout << "Opening URL...\n";

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
    
    std::string url = "https://127.0.0.1:443";

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
    publicKey = rand() % 99999;

    helloMessage["type"] = "signed_data";
    helloMessage["data"]["type"] = "hello";
    helloMessage["data"]["public_key"] = std::to_string(publicKey); // TEMP
    helloMessage["counter"] = counter;
    counter++;

    asioClient.send(global_hdl, to_string(helloMessage), websocketpp::frame::opcode::text);

    // ask for client list 
    Json clientListMessage; 
    clientListMessage["type"] = "client_list_request";

    asioClient.send(global_hdl, to_string(clientListMessage), websocketpp::frame::opcode::text);

    chatApplication->SetConnectedState(CS_CONNECTED);
}

/// ClientSocket functions
void ClientSocket::Start(std::string finalAddress) {

    try {

        chatApplication->SetConnectedState(CS_IN_PROGRESS);

        // Set logging to be pretty verbose (everything except message payloads)
        asioClient.set_access_channels(websocketpp::log::alevel::all);
        asioClient.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Initialize the Asio transport policy
        asioClient.init_asio();

        asioClient.set_message_handler(bind(&ClientSocket::OnMessage, this, placeholders::_1, placeholders::_2));
        asioClient.set_open_handler(bind(&ClientSocket::OnOpen, this, placeholders::_1));
        asioClient.set_close_handler(bind(&ClientSocket::OnClose, this, placeholders::_1));

        // Create a connection to the server
        websocketpp::lib::error_code ec;
        AsioClient::connection_ptr con = asioClient.get_connection("ws://" + finalAddress, ec); // NOT ENCRYPTED

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

int ClientSocket::SendChatMessage(string chatMessage) {

    // Creating json
    Json jsonMessage;

    Encryption encryptor;

    // Base64 Encoding Test
    string testEncode = mine::Base64::encode(chatMessage);
    cout << testEncode << endl;
    string testDecode = mine::Base64::decode(testEncode);
    cout << testDecode << endl;

    // key, worry about storage later
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
        std::cerr << "Encryption failed." << std::endl;
        return 1;
    }

    cout << encryptor.VectorToString(ciphertext) << endl;

    std::vector<unsigned char> decrypted_text;
    if (!encryptor.AESDecrypt(ciphertext, key, iv, tag, decrypted_text)) {
        std::cerr << "Decryption failed." << std::endl;
        return 1;
    }

    cout << encryptor.VectorToString(decrypted_text) << endl;

    std::string publicKey, privateKey;
    if (!encryptor.GenerateRSAKeyPair(publicKey, privateKey)) {
        std::cerr << "Failed to generate RSA key pair." << std::endl;
        return 1;
    }

    plaintext = encryptor.StringToVector(chatMessage);

    ciphertext = {};
    if (!encryptor.RSAEncrypt(plaintext, publicKey, ciphertext)) {
        std::cerr << "Encryption failed." << std::endl;
        return 1;
    }

    cout << encryptor.VectorToString(ciphertext) << endl;

    std::vector<unsigned char> decryptedText;
    if (!encryptor.RSADecrypt(ciphertext, privateKey, decryptedText)) {
        std::cerr << "Decryption failed." << std::endl;
        return 1;
    }

    cout << encryptor.VectorToString(decryptedText) << endl;

    jsonMessage["type"] = "signed_data";
    jsonMessage["data"]["type"] = "chat";
    jsonMessage["counter"] = counter; // TEMP
    jsonMessage["signature"] = "NEED TO DO"; // TEMP

    jsonMessage["data"]["destination_servers"] = uniqueServerList; //e.g. {"127.0.0.1:1", "127.0.0.1:2", "127.0.0.1:3"};

    // Chat part
    jsonMessage["data"]["chat"] = chatMessage;

    asioClient.send(global_hdl, to_string(jsonMessage), websocketpp::frame::opcode::text);

    std::cout << "message sent\n";

    return 0;
}

