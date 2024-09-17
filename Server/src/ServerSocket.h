#pragma once

#include <iostream>
#include <list>
#include <thread>

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

#include <json.hpp> 

typedef websocketpp::client<websocketpp::config::asio_client> Client;

class ServerSocket {
    public:
        void ConnectToServer(std::string, std::string);
        void SendJson(nlohmann::json);

        std::string GetConnectionAddress();
        
        ServerSocket();

    private:
        Client client;
        websocketpp::connection_hdl connection;

        void SendPayload();
        void OnOpen(websocketpp::connection_hdl);
        void OnMessage(websocketpp::connection_hdl, websocketpp::config::asio_client::message_type::ptr);

        std::string connectionAddress;
        std::string hostAddress;
};