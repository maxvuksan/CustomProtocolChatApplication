#pragma once

#include "ChatMessage.h"
#include "Encryption.h"
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

        int SendChatMessage(std::string chatMessage, std::string publicKey);

        // allow the client socket to talk to the UI 
        void SetChatApplication(ChatApplication* ref){this->chatApplication = ref;}
        void SetClient(Client* client){this->client = client;}

        void OnMessage(websocketpp::connection_hdl hdl, websocketpp::config::asio_client::message_type::ptr msg);
        void ParseMessage(const std::string& data);

        void OnClose(websocketpp::connection_hdl hdl);
        void OnOpen(websocketpp::connection_hdl hdl);

        static void OpenLinkInBrowser(std::string url);
        void SelectFile();
        bool UploadFileToServer(const std::string& filepath);

        //int GetPublicKey(){return publicKey;};
        std::string publicKey;
    private:

        std::string privateKey;
        std::string fingerprint;
        
        Encryption encryptor;

        // where all the clients are from
        std::vector<std::string> uniqueServerList;

        Client* client;
        ChatApplication* chatApplication;
        
        // should be reset everytime we begin a new connection
        int counter = 0;
        
        websocketpp::connection_hdl global_hdl;
        AsioClient asioClient;

};