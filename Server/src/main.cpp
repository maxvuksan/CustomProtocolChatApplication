#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <iostream>

typedef websocketpp::server<websocketpp::config::asio> server_type;

// Define a callback for handling messages
void on_message(server_type* s, websocketpp::connection_hdl hdl, server_type::message_ptr msg) {

    std:: cout << "Recieved payload!" << std::endl;
    std::cout << msg->get_payload() << std::endl;
    
    s->send(hdl, "Message Recieved", msg->get_opcode());
}

int main() {

    try {
        // Create a server endpoint
        server_type server;

        // Set logging settings (optional)
        server.set_access_channels(websocketpp::log::alevel::all);
        server.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Set the message handler
        server.set_message_handler(websocketpp::lib::bind(&on_message, &server, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));

        // Set the listening port
        server.init_asio();
        server.listen(9002);
        server.start_accept();

        std::cout << "starting server\n";

        // Start the ASIO io_service loop
        server.run();

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}