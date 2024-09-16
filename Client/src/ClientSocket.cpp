#include "ClientSocket.h"
#include "ChatMessage.h"
#include <iostream>
#include <json.hpp>
#include "ChatApplication.h"


using namespace std;
using Json = nlohmann::json;

/// Handlers
void ClientSocket::OnMessage(websocketpp::connection_hdl hdl, websocketpp::config::asio_client::message_type::ptr msg) {
    std::cout << "Received message: " << msg->get_payload() << std::endl;
}

void ClientSocket::ParseMessage(const std::string& data){






}

// Define a close handler function
void ClientSocket::OnClose(websocketpp::connection_hdl hdl) {
    std::cout << "Connection closed" << std::endl;

    chatApplication->SetConnectedState(CS_DISCONNECTED);
}

void ClientSocket::OnOpen(websocketpp::connection_hdl hdl) {
    global_hdl = hdl;

    std::cout << "Connection opened!" << std::endl;

    // Example: Send a message to the server right after the connection opens
    websocketpp::lib::error_code ec;
    
    //c.send(hdl, message, websocketpp::frame::opcode::text, ec);

    // Creating json
    Json jsonMessage;

    jsonMessage["type"] = "signed_data";
    jsonMessage["data"]["type"] = "hello";
    jsonMessage["data"]["public_key"] = "0123"; // TEMP

    c.send(global_hdl, to_string(jsonMessage), websocketpp::frame::opcode::text);



    chatApplication->SetConnectedState(CS_CONNECTED);
}

/// ClientSocket functions
void ClientSocket::Start(std::string finalAddress) {
    try {

        chatApplication->SetConnectedState(CS_IN_PROGRESS);

        // Set logging to be pretty verbose (everything except message payloads)
        c.set_access_channels(websocketpp::log::alevel::all);
        c.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Initialize the Asio transport policy
        c.init_asio();

        c.set_message_handler(bind(&ClientSocket::OnMessage, this, placeholders::_1, placeholders::_2));
        c.set_open_handler(bind(&ClientSocket::OnOpen, this, placeholders::_1));
        c.set_close_handler(bind(&ClientSocket::OnClose, this, placeholders::_1));

        // Create a connection to the server
        websocketpp::lib::error_code ec;
        AsioClient::connection_ptr con = c.get_connection("ws://" + finalAddress, ec); // NOT ENCRYPTED

        if (ec) {
            std::cout << "Could not create connection: " << ec.message() << std::endl;
            return;
        }

        // Connect the connection
        c.connect(con);

        // Start the ASIO io_service run loop
        c.run();



    } catch (websocketpp::exception const & e) {
        std::cout << e.what() << std::endl;
    } catch (...) {
        std::cout << "Unknown exception" << std::endl;
    }    
}


void ClientSocket::End() {
    c.close(global_hdl, websocketpp::close::status::normal, "Client closing connection");
}

int ClientSocket::SendChatMessage(ChatMessage chatMessage) {

    // Creating json
    Json jsonMessage;

    jsonMessage["type"] = "signed_data";
    jsonMessage["data"]["type"] = "chat";
    jsonMessage["counter"] = "NEED TO DO"; // TEMP
    jsonMessage["signature"] = "NEED TO DO"; // TEMP

    // Chat part
    jsonMessage["data"]["chat"] = chatMessage.message;

    c.send(global_hdl, to_string(jsonMessage), websocketpp::frame::opcode::text);

    return 0;
}

