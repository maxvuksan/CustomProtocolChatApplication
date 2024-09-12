#include "ServerHost.h"

using namespace std;

using Json = nlohmann::json;

// Define a callback for handling messages
void on_message(server_type* s, websocketpp::connection_hdl hdl, server_type::message_ptr msg) {
    // s->send(hdl, msg->get_payload(), msg->get_opcode());

    Json json = Json::parse(msg->get_payload());

    cout << json << endl;
}

void OnOpen(websocketpp::connection_hdl hdl) {
    cout << "Someone connected to server!" << endl;
}

void ServerHost::StartServer(int port) {
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

        server.set_open_handler(&OnOpen);

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