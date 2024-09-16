#include "ServerHost.h"

using namespace std;

using Json = nlohmann::json;

// Define a callback for handling messages
void ServerHost::OnMessage(websocketpp::connection_hdl hdl, server_type::message_ptr msg) {
    Json json = Json::parse(msg->get_payload());
    string type = json["data"]["type"];

    if (type == "hello") {
        string publicKey = json["data"]["public_key"];
        AddClient(publicKey);
    }

    if (type == "server_hello") {
        cout << json["data"]["sender"] << endl;
    }

    if (type == "client_update") {
        cout << json["data"]["clients"] << endl;
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

    jsonMessage["type"] = "data";
    jsonMessage["data"]["type"] = "client_update";
    jsonMessage["data"]["clients"] = Json::array({1, 2, 3, 4});

    for (list<ServerSocket>::iterator it = serverSockets->begin(); it != serverSockets->end(); ++it) {
        it->SendJson(jsonMessage);
    }
}