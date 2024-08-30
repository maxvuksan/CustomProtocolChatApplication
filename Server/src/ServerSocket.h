#pragma once

#include <iostream>

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

class ServerSocket {
    public:
        int ConnectToServer(std::string ipAddress);

    private:

};