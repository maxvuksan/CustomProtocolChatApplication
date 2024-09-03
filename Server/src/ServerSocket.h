#pragma once

#include <iostream>

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

typedef websocketpp::client<websocketpp::config::asio_client> Client;

class ServerSocket {
    public:
        void ConnectToServer(std::string);

    private:
        

};