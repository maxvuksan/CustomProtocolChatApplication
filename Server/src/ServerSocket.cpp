#include "ServerSocket.h"

using Json = nlohmann::json; 


using namespace std;

void ServerSocket::OnMessage(websocketpp::connection_hdl hdl, websocketpp::config::asio_client::message_type::ptr payload) {

}

void ServerSocket::OnOpen(websocketpp::connection_hdl hdl) {
    connection = hdl;

    Json jsonHello;

    jsonHello["type"] = "signed_data";
    jsonHello["data"]["type"] = "server_hello";
    jsonHello["data"]["sender"] = hostAddress;

    SendJson(jsonHello);

    Json jsonClientRequest;

    jsonClientRequest["type"] = "client_update_request";

    SendJson(jsonClientRequest);
}

void ServerSocket::OnFail(websocketpp::connection_hdl hdl) {

}

ServerSocket::ServerSocket() {
    client.set_access_channels(websocketpp::log::alevel::all);
    client.clear_access_channels(websocketpp::log::alevel::frame_payload);

    client.init_asio();

    client.set_message_handler(bind(&ServerSocket::OnMessage, this, placeholders::_1, placeholders::_2));
    client.set_open_handler(bind(&ServerSocket::OnOpen, this, placeholders::_1));
    client.set_fail_handler(bind(&ServerSocket::OnFail, this, placeholders::_1));
}

void ServerSocket::ConnectToServer(string dstIp, string srcAddress) {

    hostAddress = srcAddress;
    connectionAddress = dstIp;

    websocketpp::lib::error_code ec; 
    
    cout << "Attempting to connect to server with ip: " << dstIp << endl;

    string address = "ws://" + dstIp;

    
    

        Client::connection_ptr con = client.get_connection(address, ec);
    
        cout << ec << endl;

        if (ec) {  
            cout << "Connection attempt failed: " << ec.message() << endl;
        } else {
            client.connect(con);
            client.run();
        }

       



    return;
}

void ServerSocket::SendJson(Json json) {
    client.send(connection, to_string(json), websocketpp::frame::opcode::text);
}

string ServerSocket::GetConnectionAddress() {
    return connectionAddress;
}