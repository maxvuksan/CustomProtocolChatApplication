#include "Server.h"




void Server::Run(){

    std::cout << "Starting Server...\n";

    try {
        // Set logging settings (optional)
        server.set_access_channels(websocketpp::log::alevel::all);
        server.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Set the message handler
        server.set_message_handler([this](websocketpp::connection_hdl hdl, server_type::message_ptr msg) {
            this->Callback_OnMessage(hdl, msg);
        });

        // Set the listening port
        server.init_asio();
        server.listen(9002);
        server.start_accept();

        // Start the ASIO io_service loop
        server.run();

    } 
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}



void Server::Callback_OnMessage(websocketpp::connection_hdl connection_hdl, server_type::message_ptr msg) {

    server.send(connection_hdl, msg->get_payload(), msg->get_opcode());
}
