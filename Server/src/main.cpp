#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <iostream>

typedef websocketpp::server<websocketpp::config::asio> server_type;

// Define a callback for handling messages
void on_message(server_type* s, websocketpp::connection_hdl hdl, server_type::message_ptr msg) {
    s->send(hdl, msg->get_payload(), msg->get_opcode());
}

int main() {

    std::mutex adad;

    try {
        // Create a server endpoint
        server_type server;

        // Set logging settings (optional)
        server.set_access_channels(websocketpp::log::alevel::all);
        server.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Set the message handler
        server.set_message_handler([&server](websocketpp::connection_hdl hdl, server_type::message_ptr msg) {
            on_message(&server, hdl, msg);
        });

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