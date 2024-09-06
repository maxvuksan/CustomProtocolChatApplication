#include "ServerSocket.h"

using namespace std;


ServerSocket::ServerSocket() {
    client.set_access_channels(websocketpp::log::alevel::all);
    client.clear_access_channels(websocketpp::log::alevel::frame_payload);

    client.init_asio();
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



    // try {
    //         // Set logging to be pretty verbose (everything except message payloads)
    //         client.set_access_channels(websocketpp::log::alevel::all);
    //         client.clear_access_channels(websocketpp::log::alevel::frame_payload);

    //         // Initialize the Asio transport policy
    //         client.init_asio();
    
    //         string address = "ws://" + ip;

    //         // Create a connection to the server
    //         websocketpp::lib::error_code ec;
    //         Client::connection_ptr connection = client.get_connection(address, ec); // NOT ENCRYPTED

    //         if (ec) {
    //             std::cout << "Could not create connection: " << ec.message() << std::endl;
    //             return;
    //         }

    //         // Connect the connection
    //         client.connect(connection);

    //         // Start the ASIO io_service run loop
    //         client.run();
    //     } catch (websocketpp::exception const & e) {
    //         std::cout << e.what() << std::endl;
    //     } catch (...) {
    //         std::cout << "Unknown exception" << std::endl;
    //     }    

    return;
}