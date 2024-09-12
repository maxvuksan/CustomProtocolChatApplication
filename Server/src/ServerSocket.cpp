#include "ServerSocket.h"

#include <json.hpp> // TEMP DELETE
using Json = nlohmann::json; // TEMP DELETE

using namespace std;

void OnMessage(websocketpp::connection_hdl hdl, websocketpp::config::asio_client::message_type::ptr payload) {
    cout << "Recieved a message!" << endl;
}

void ServerSocket::OnOpen(websocketpp::connection_hdl hdl) {
    connection = hdl;
    cout << "Connection Opened" << endl;
    SendPayload();
}

ServerSocket::ServerSocket() {
    client.set_access_channels(websocketpp::log::alevel::all);
    client.clear_access_channels(websocketpp::log::alevel::frame_payload);

    client.init_asio();

    client.set_message_handler(&OnMessage);
    client.set_open_handler(bind(&ServerSocket::OnOpen, this, placeholders::_1));
}

void ServerSocket::ConnectToServer(string ip) {
    websocketpp::lib::error_code ec; 
    
    cout << "Attempting to connect to server with ip: " << ip << endl;

    string address = "ws://" + ip;
    Client::connection_ptr con = client.get_connection(address, ec);
    
    if (ec) {
        cout << "Error" << endl;
        return;
    }

    client.connect(con);
    client.run();

    return;
}

/// TEMP MIMICING WHAT A CLIENT SENDS ON CONNECTION
void ServerSocket::SendPayload() {
    // Creating json
    Json jsonMessage;

    jsonMessage["type"] = "data";
    jsonMessage["data"]["type"] = "hello";
    jsonMessage["public_key"] = "1";

    client.send(connection, to_string(jsonMessage), websocketpp::frame::opcode::text);
}