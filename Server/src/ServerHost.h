#pragma once

#include "iostream"

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <json.hpp>

typedef websocketpp::server<websocketpp::config::asio> server_type;

class ServerHost {

    public:
        void StartServer(int);
        void Callback_OnMessage(websocketpp::connection_hdl connection_hdl, server_type::message_ptr message);

    private:
        
};