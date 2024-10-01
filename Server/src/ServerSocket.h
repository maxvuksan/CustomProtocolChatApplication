#pragma once

#include <iostream>
#include <list>
#include <thread>
#include <unistd.h>

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

#include <json.hpp> 

#include "Encryption.h"

typedef websocketpp::client<websocketpp::config::asio_tls_client> Client;

class ServerSocket {

    public:
    
        void ConnectToServer(std::string, std::string, std::string);
        void SendJson(nlohmann::json);
        bool IsConnected();
        void Stop();

        std::string GetConnectionAddress();
        
        ServerSocket();
        ~ServerSocket();

    private:
        Client client;
        websocketpp::connection_hdl connection;

        void SendPayload();

        void OnOpen(websocketpp::connection_hdl);
        void OnMessage(websocketpp::connection_hdl, websocketpp::config::asio_client::message_type::ptr);
        void OnFail(websocketpp::connection_hdl);
        void OnClose(websocketpp::connection_hdl);

        std::string connectionAddress;
        std::string hostAddress;

        bool connected;
        int counter = 0;

        std::string publicKey;

        Encryption encryptor;
};