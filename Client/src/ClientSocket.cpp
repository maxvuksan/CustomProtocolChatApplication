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
    ParseMessage(msg->get_payload());
}

void ClientSocket::ParseMessage(const std::string& data){

    std::cout << data << "\n";

    Json json = Json::parse(data);
    
    string topmostType = json["type"];


    if(topmostType == "signed_data"){

        // handle data messages

    }
    if(topmostType == "client_list"){  

        // TEMP : Clears the whole list of clients, should instead determine which clients should be updated and only update those

        client->ClearActiveUsers();

        int serverI = 0;
        for(auto serverInput : json["servers"]){
            
            for(auto clientInput : json["servers"][serverI]["clients"]){

                client->PushActiveUser(clientInput);

            }
            serverI++;
        }
    }

}

// Define a close handler function
void ClientSocket::OnClose(websocketpp::connection_hdl hdl) {
    std::cout << "Connection closed" << std::endl;

    chatApplication->SetConnectedState(CS_DISCONNECTED);
}

void ClientSocket::OnOpen(websocketpp::connection_hdl hdl) {

    global_hdl = hdl;

    std::cout << "Connection opened!" << std::endl;

    this->counter = 0;

    // Example: Send a message to the server right after the connection opens
    websocketpp::lib::error_code ec;
    

    // Creating json 
    Json helloMessage;

    helloMessage["type"] = "signed_data";
    helloMessage["data"]["type"] = "hello";
    helloMessage["data"]["public_key"] = "0123"; // TEMP
    helloMessage["counter"] = counter;
    counter++;

    asioClient.send(global_hdl, to_string(helloMessage), websocketpp::frame::opcode::text);

    // ask for client list 
    Json clientListMessage; 
    clientListMessage["type"] = "client_list_request";

    asioClient.send(global_hdl, to_string(clientListMessage), websocketpp::frame::opcode::text);

    chatApplication->SetConnectedState(CS_CONNECTED);
}

/// ClientSocket functions
void ClientSocket::Start(std::string finalAddress) {
    try {

        chatApplication->SetConnectedState(CS_IN_PROGRESS);

        // Set logging to be pretty verbose (everything except message payloads)
        asioClient.set_access_channels(websocketpp::log::alevel::all);
        asioClient.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Initialize the Asio transport policy
        asioClient.init_asio();

        asioClient.set_message_handler(bind(&ClientSocket::OnMessage, this, placeholders::_1, placeholders::_2));
        asioClient.set_open_handler(bind(&ClientSocket::OnOpen, this, placeholders::_1));
        asioClient.set_close_handler(bind(&ClientSocket::OnClose, this, placeholders::_1));

        // Create a connection to the server
        websocketpp::lib::error_code ec;
        AsioClient::connection_ptr con = asioClient.get_connection("ws://" + finalAddress, ec); // NOT ENCRYPTED

        if (ec) {
            std::cout << "Could not create connection: " << ec.message() << std::endl;
            return;
        }

        // Connect the connection
        asioClient.connect(con);

        // Start the ASIO io_service run loop
        asioClient.run();



    } catch (websocketpp::exception const & e) {
        std::cout << e.what() << std::endl;
    } catch (...) {
        std::cout << "Unknown exception" << std::endl;
    }    
}


void ClientSocket::End() {
    asioClient.close(global_hdl, websocketpp::close::status::normal, "Client closing connection");
}

int ClientSocket::SendChatMessage(ChatMessage chatMessage) {

    // Creating json
    Json jsonMessage;

    jsonMessage["type"] = "signed_data";
    jsonMessage["data"]["type"] = "chat";
    jsonMessage["counter"] = counter; // TEMP
    jsonMessage["signature"] = "NEED TO DO"; // TEMP

    // Chat part
    jsonMessage["data"]["chat"] = chatMessage.message;

    asioClient.send(global_hdl, to_string(jsonMessage), websocketpp::frame::opcode::text);

    return 0;
}

