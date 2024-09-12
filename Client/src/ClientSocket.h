#pragma once

#include "ChatMessage.h"
#include <thread>

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

typedef websocketpp::client<websocketpp::config::asio_client> AsioClient;

class ChatApplication;
class ClientSocket {
    public:
        void Start(std::string finalAddress);
        void End();

        int SendChatMessage(ChatMessage chatMessage);

        // allow the client socket to talk to the UI 
        void SetChatApplication(ChatApplication* ref){this->chatApplication = ref;}


        void OnMessage(websocketpp::connection_hdl hdl, websocketpp::config::asio_client::message_type::ptr msg);
        void OnClose(websocketpp::connection_hdl hdl);
        void OnOpen(websocketpp::connection_hdl hdl);

    private:


        ChatApplication* chatApplication;
        
        websocketpp::connection_hdl global_hdl;
        AsioClient c;

};