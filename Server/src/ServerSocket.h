#pragma once

#include <iostream>
#include <list>
#include <thread>

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

typedef websocketpp::client<websocketpp::config::asio_client> Client;

class ServerSocket {
    public:
        void ConnectToServer(std::string);

        ServerSocket();

    private:
        Client client;
        websocketpp::connection_hdl connection;
        

};