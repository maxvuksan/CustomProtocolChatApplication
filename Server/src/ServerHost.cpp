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
        
    }

    // Signed data
    if (type != "signed_data") {
        return;
    }

    type = json["data"]["type"];

    if (type == "hello") {
        string publicKey = json["data"]["public_key"];
        AddClient(publicKey);
    }

    if (type == "server_hello") {
        AddNewExternalClientList(hdl, json["data"]["sender"]);
    }


}

void ServerHost::SendAllClientLists(websocketpp::connection_hdl connection) {
    Json jsonMessage;

    jsonMessage["type"] = "client_list";
    
}

void ServerHost::AddNewExternalClientList(websocketpp::connection_hdl connection, string address) {

    ClientList externalClientListElement;
    externalClientListElement.connection = connection;
    externalClientListElement.address = address;

    externalClientLists.push_back(externalClientListElement);
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
}

void ServerHost::StartServer(int port, list<ServerSocket> * socketList) {
    try {
        // Create a server endpoint
        server_type server;

        serverSockets = socketList;

        // Set logging settings (optional)
        server.set_access_channels(websocketpp::log::alevel::all);
        server.clear_access_channels(websocketpp::log::alevel::frame_payload);

        server.set_message_handler(bind(&ServerHost::OnMessage, this, placeholders::_1, placeholders::_2));


        // Set the listening port
        server.init_asio();
        server.listen(port);
        server.start_accept();

        cout << "Server started on port: " << port << endl; 

        // Start the ASIO io_service loop
        server.run();

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return;
    }

    return;
}

void ServerHost::AddClient(string publicKey) {
    clientList.emplace_back(publicKey);

    SendClientUpdate();
}

void ServerHost::SendClientUpdate() {

    Json jsonMessage;

    jsonMessage["type"] = "client_update";
    jsonMessage["clients"] = {};

    for (list<string>::iterator it = clientList.begin(); it != clientList.end(); it++) {
        jsonMessage["clients"].push_back(*it);
    }

    for (list<ServerSocket>::iterator it = serverSockets->begin(); it != serverSockets->end(); ++it) {
        it->SendJson(jsonMessage);
    }
}