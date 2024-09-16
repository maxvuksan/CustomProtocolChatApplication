#pragma once

#include "ChatMessage.h"
#include <thread>

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

typedef websocketpp::client<websocketpp::config::asio_client> AsioClient;

class Client;
class ChatApplication;

class ClientSocket {
    public:
        void Start(std::string finalAddress);
        void End();

        int SendChatMessage(ChatMessage chatMessage);

        // allow the client socket to talk to the UI 
        void SetChatApplication(ChatApplication* ref){this->chatApplication = ref;}
        void SetClient(Client* client){this->client = client;}

        void OnMessage(websocketpp::connection_hdl hdl, websocketpp::config::asio_client::message_type::ptr msg);
        void ParseMessage(const std::string& data);

        void OnClose(websocketpp::connection_hdl hdl);
        void OnOpen(websocketpp::connection_hdl hdl);

    private:


        Client* client;
        ChatApplication* chatApplication;
        
        websocketpp::connection_hdl global_hdl;
        AsioClient asioClient;

};