#include "ClientSocket.h"
#include <iostream>

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

using namespace std;

typedef websocketpp::client<websocketpp::config::asio_client> client;

websocketpp::connection_hdl global_hdl;
client c;

/// Handlers
void on_message(client* c, websocketpp::connection_hdl hdl, websocketpp::config::asio_client::message_type::ptr msg) {
    std::cout << "Received message: " << msg->get_payload() << std::endl;
}

// Define a close handler function
void on_close(client* c, websocketpp::connection_hdl hdl) {
    std::cout << "Connection closed" << std::endl;
}

void on_open(client* c, websocketpp::connection_hdl hdl) {
    global_hdl = hdl;

    std::cout << "Connection opened!" << std::endl;

    // Example: Send a message to the server right after the connection opens
    websocketpp::lib::error_code ec;
    std::string message = "Hello, WebSocket server! This is a test";
    c->send(hdl, message, websocketpp::frame::opcode::text, ec);
    if (ec) {
        std::cout << "Error sending message: " << ec.message() << std::endl;
    } else {
        std::cout << "Message sent: " << message << std::endl;
    }
}

/// ClientSocket functions
void ClientSocket::Start() {
    try {
        // Set logging to be pretty verbose (everything except message payloads)
        c.set_access_channels(websocketpp::log::alevel::all);
        c.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Initialize the Asio transport policy
        c.init_asio();

        // Register our message handler
        c.set_message_handler(websocketpp::lib::bind(&on_message, &c, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));

        // Register our close handler
        c.set_close_handler(websocketpp::lib::bind(&on_close, &c, websocketpp::lib::placeholders::_1));

        c.set_open_handler(websocketpp::lib::bind(&on_open, &c, websocketpp::lib::placeholders::_1));


        // Create a connection to the server
        websocketpp::lib::error_code ec;
        client::connection_ptr con = c.get_connection("ws://192.168.1.109:9002", ec); // NOT ENCRYPTED

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

void ClientSocket::SendPayload() {
    c.send(global_hdl, "Foo", websocketpp::frame::opcode::text);
}

