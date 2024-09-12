#pragma once

#include "iostream"

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include "ServerSocket.h"

#include <json.hpp>
#include <list>

typedef websocketpp::server<websocketpp::config::asio> server_type;

class ServerHost {

    public:
        void StartServer(int, std::list<ServerSocket> *);
        void Callback_OnMessage(websocketpp::connection_hdl connection_hdl, server_type::message_ptr message);

    private:
        void OnMessage(websocketpp::connection_hdl, server_type::message_ptr);
        void OnOpen();

        void AddClient(std::string);
        void SendClientUpdate();

        std::list<std::string> clientList;
        std::list<ServerSocket> * serverSockets;
};