#include "ServerHost.h"

using namespace std;

using Json = nlohmann::json;

// Define a callback for handling messages
void ServerHost::OnMessage(websocketpp::connection_hdl hdl, server_type::message_ptr msg) {
    Json json = Json::parse(msg->get_payload());
    string type = json["type"];

    // Unsigned data
    if (type == "client_update") {
        
        UpdateExternalClientList(hdl, json["clients"]);
    }

    if (type == "client_list_request") {
        cout << "Sending client list" << endl;
        SendAllClientLists(hdl);
    }

    if (type == "client_update_request") {
        // Do not send client list if server did not pass the server_hello
        if (!CheckIfHdlMatches(serverConnections, hdl)) {
            server.close(hdl, websocketpp::close::status::normal, "Not whitelisted");
            return;
        }

        SendClientUpdate();
    }

    // Signed data
    if (type != "signed_data") {
        return;
    }

    type = json["data"]["type"];
    
    if (type == "hello") {
        string publicKey = json["data"]["public_key"];
        AddClient(publicKey, hdl);
    }

    if (type == "server_hello") {
        bool isValid = CheckServerSignature(json["signature"]);

        if (isValid != true) {
            server.close(hdl, websocketpp::close::status::normal, "Not whitelisted");
        }

        serverConnections.push_back(hdl);
        AddNewExternalClientList(hdl, json["data"]["sender"]);
    }

    int counter = json["counter"];
  
    bool isValid = ValidCount(hdl, counter);
    if (!isValid) {
        server.close(hdl, websocketpp::close::status::normal, "Invalid structure");
        return;
    }

    if (type == "chat") {
        Json addresses = json["data"]["destination_servers"];
        SendChatMessage(json, addresses, hdl);
    }

    if (type == "public_chat") {
        SendPublicChatMessage(json);    

        for (list<websocketpp::connection_hdl>::iterator it = clientConnections.begin(); it != clientConnections.end(); it++) {
            if (hdl.lock() == it->lock()) {

                for (list<ServerSocket>::iterator il = serverSockets->begin(); il != serverSockets->end(); ++il) {
                    if (il->IsConnected() != true) {
                        continue;
                    }

                    il->SendJson(json);
                }

            }
        }   
        
    }


}

bool ServerHost::ValidCount(websocketpp::connection_hdl hdl, int count) {

    int iterator = 0;
    for (list<websocketpp::connection_hdl>::iterator it = clientConnections.begin(); it != clientConnections.end(); it++) {
        if (hdl.lock() != it->lock()) {
            iterator++;
            continue;
        }

        if (count > *next(clientCounts.begin(), iterator)) {
            *next(clientCounts.begin(), iterator) = count;
            return true;
        }

        return false;
    }    

    iterator = 0;
    for (list<ClientList>::iterator it = externalClientLists.begin(); it != externalClientLists.end(); it++) {
        if (hdl.lock() != it->connection.lock()) {
            iterator++;
            continue;
        }

        return true;
    }

    return false;

}

bool ServerHost::CheckIfHdlMatches(list<websocketpp::connection_hdl> connectionList, websocketpp::connection_hdl hdl) {

    for (list<websocketpp::connection_hdl>::iterator it = connectionList.begin(); it != connectionList.end(); it++) {
        if (hdl.lock() != it->lock()) {
            continue;
        }

        return true;
    }

    return false;
    
}

bool ServerHost::CheckServerSignature(string hash) {
    ifstream file("Server Whitelist.txt");
    if (!file.is_open()) {
        return -1;
    }

    string line;
    bool started = false;
    string key = "";
    while (getline(file, line)) {
        if (line.find("-----BEGIN PUBLIC KEY-----") != -1) {
            started = true;
            key += line + "\n";
            continue;
        }

        if (line.find("-----END PUBLIC KEY-----") != -1) {
            started = false;
            key += line;

            vector<unsigned char> fingerprintVector;
            if (!encryptor.CreateFingerprint(key, fingerprintVector)) {
                cerr << "Fingerprint failed" << endl;
            }

            string fingerprint = mine::Base64::encode(encryptor.VectorToString(fingerprintVector));
            
            if (fingerprint == hash) {
                return true;
            }

            key = "";
            continue;
        }

        if (started = true) {
            key += line + "\n";
        }
    }
    file.close();

    return false;
}

void ServerHost::OnClose(websocketpp::connection_hdl connection) {

    // Client section
    bool remove = false;
    int count = 0;
    for (list<websocketpp::connection_hdl>::iterator it = clientConnections.begin(); it != clientConnections.end(); it++) {
        
        if (connection.lock() == it->lock()) {
            remove = true;
            break;
        }

        count++;
    }

    if (remove == true) {
        clientList.erase(next(clientList.begin(), count));
        clientConnections.erase(next(clientConnections.begin(), count));
        clientCounts.erase(next(clientCounts.begin(), count));

        SendClientUpdate();
        SendAllClientListsToAllClients();

        return;
    }

    remove = false;
    count = 0;
    for (list<ClientList>::iterator it = externalClientLists.begin(); it != externalClientLists.end(); it++) {
        if (connection.lock() == it->connection.lock()) {
            remove = true;
            break;
        }
        
        count++;
    }

    if (remove == true) {

        externalClientLists.erase(next(externalClientLists.begin(), count));
        serverCounts.erase(next(serverCounts.begin(), count));
        foo.erase(next(foo.begin(), count));


        SendAllClientListsToAllClients();

        return;
    }

    
}

void ServerHost::SendPublicChatMessage(Json message) {

    for (list<websocketpp::connection_hdl>::iterator it = clientConnections.begin(); it != clientConnections.end(); it++) {
        server.send(*it, to_string(message), websocketpp::frame::opcode::text);
    }

}

void ServerHost::SendChatMessage(Json message, Json addresses, websocketpp::connection_hdl hdl) {
     for (auto& x : addresses.items()) {
        string address = x.value();

        // Remove destination addresses from the message (Stops looping)
        // message["data"]["destination_servers"] = { address };

        if (address == myAddress) { // Send message to all clients
            
            for (list<websocketpp::connection_hdl>::iterator it = clientConnections.begin(); it != clientConnections.end(); it++) {
                server.send(*it, to_string(message), websocketpp::frame::opcode::text);
            }
        } else if (CheckIfHdlMatches(clientConnections, hdl)) {                    // Send message to destination servers

            for (list<ServerSocket>::iterator it = serverSockets->begin(); it != serverSockets->end(); it++) {
                if (it->IsConnected() != true) {
                    continue;
                }

                if (it->GetConnectionAddress() != address) {
                    continue;
                }

                it->SendJson(message);
            }

        }

     }

     
}

void ServerHost::SendAllClientLists(websocketpp::connection_hdl connection) {
    Json jsonMessage;

    jsonMessage["type"] = "client_list";
    jsonMessage["servers"] = {};

    Json myServer;
    myServer["address"] = myAddress;
    myServer["clients"] = {}; 
    for (list<string>::iterator it = clientList.begin(); it != clientList.end(); it++) {
        myServer["clients"].push_back(*it);
    }

    jsonMessage["servers"].push_back(myServer);

    for (list<ClientList>::iterator it = externalClientLists.begin(); it != externalClientLists.end(); it++) {
        Json server;
        server["address"] = it->address;
        server["clients"] = {};

        for (list<string>::iterator il = it->clientList.begin(); il != it->clientList.end(); il++) {
            server["clients"].push_back(*il);
        }

        jsonMessage["servers"].push_back(server);
    }

    server.send(connection, to_string(jsonMessage), websocketpp::frame::opcode::text);
    
}

void ServerHost::SendAllClientListsToAllClients() {
    Json jsonMessage;

    jsonMessage["type"] = "client_list";
    jsonMessage["servers"] = {};

    Json myServer;
    myServer["address"] = myAddress;
    myServer["clients"] = {}; 
    for (list<string>::iterator it = clientList.begin(); it != clientList.end(); it++) {
        myServer["clients"].push_back(*it);
    }

    jsonMessage["servers"].push_back(myServer);

    for (list<ClientList>::iterator it = externalClientLists.begin(); it != externalClientLists.end(); it++) {

        Json server;
        server["address"] = it->address;
        server["clients"] = {};

        for (list<string>::iterator il = it->clientList.begin(); il != it->clientList.end(); il++) {
            server["clients"].push_back(*il);
        }

        jsonMessage["servers"].push_back(server);
    }

    for (list<websocketpp::connection_hdl>::iterator it = clientConnections.begin(); it != clientConnections.end(); it++) {
        server.send(*it, to_string(jsonMessage), websocketpp::frame::opcode::text);
    }
}

void ServerHost::AddNewExternalClientList(websocketpp::connection_hdl connection, string address) {

    ClientList externalClientListElement;
    externalClientListElement.connection = connection;
    externalClientListElement.address = address;

    externalClientLists.push_back(externalClientListElement);
    serverCounts.push_back(-1);

    cout << "Added server: " << count << endl;
    foo.push_back(count);
    count++;


    Json jsonClientRequest;
    jsonClientRequest["type"] = "client_update_request";
    
    for (list<ServerSocket>::iterator it = serverSockets->begin(); it != serverSockets->end(); it++) {
        if (it->IsConnected() != true) {
            continue;
        }

        it->SendJson(jsonClientRequest);
    }
};


void ServerHost::UpdateExternalClientList(websocketpp::connection_hdl connection, Json jsonArray) {

    for (list<ClientList>::iterator it = externalClientLists.begin(); it != externalClientLists.end(); it++) {
        // checking if hdl matches
        if (connection.owner_before(it->connection) || it->connection.owner_before(connection)) {
            continue;
        }

        // Overriding client list with jsonArray
        it->clientList.clear();
        for (auto& x : jsonArray.items()) {
            it->clientList.push_back(x.value());
        }
    }

    SendAllClientListsToAllClients();
}

void ServerHost::StartServer(int port, list<ServerSocket> * socketList, string address, string key) {
    try {
        // Create a server endpoint
        serverSockets = socketList;
        publicKey = key;

        // Set logging settings (optional)
        server.set_access_channels(websocketpp::log::alevel::all);
        server.clear_access_channels(websocketpp::log::alevel::frame_payload);

        server.set_message_handler(bind(&ServerHost::OnMessage, this, placeholders::_1, placeholders::_2));
        server.set_close_handler(bind(&ServerHost::OnClose, this, placeholders::_1));


        // Set the listening port
        server.init_asio();

        // Set up TLS
        string certificateFile = "-server.crt";
        string keyFile = "server.key";
        server.set_tls_init_handler([certificateFile, keyFile](websocketpp::connection_hdl hdl) -> shared_ptr<asio::ssl::context> {
            std::shared_ptr<asio::ssl::context> ctx = make_shared<asio::ssl::context>(asio::ssl::context::tlsv12);

            try {
                ctx->set_options(asio::ssl::context::default_workarounds |
                                 asio::ssl::context::no_sslv2 |
                                 asio::ssl::context::no_sslv3 |
                                 asio::ssl::context::single_dh_use);

                ctx->use_certificate_chain_file(certificateFile);
                ctx->use_private_key_file(keyFile, asio::ssl::context::pem);
            } catch (exception& e) {
                cerr << "Couldn't set up TLS: " << e.what() << endl;
            } 

            return ctx;
        });

        

        server.listen(port);
        server.start_accept();

        cout << "Server started on port: " << port << endl; 
        myAddress = address;

        // Start the ASIO io_service loop
        server.run();

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return;
    }

    return;
}

void ServerHost::AddClient(string publicKey, websocketpp::connection_hdl hdl) {
    clientList.push_back(publicKey);
    clientConnections.push_back(hdl);
    clientCounts.push_back(-1);

    SendClientUpdate();
    SendAllClientListsToAllClients();
}

void ServerHost::SendClientUpdate() {

    Json jsonMessage;

    jsonMessage["type"] = "client_update";
    jsonMessage["clients"] = {};

    for (list<string>::iterator it = clientList.begin(); it != clientList.end(); it++) {
        jsonMessage["clients"].push_back(*it);
    }

    for (list<ServerSocket>::iterator it = serverSockets->begin(); it != serverSockets->end(); ++it) {
        if (it->IsConnected() != true) {
            continue;
        }

        it->SendJson(jsonMessage);
    }
}