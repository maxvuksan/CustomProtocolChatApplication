#pragma once

#include "iostream"

#include <websocketpp/config/asio.hpp>
#include <websocketpp/server.hpp>

#include "ServerSocket.h"

#include <json.hpp>
#include <list>

typedef websocketpp::server<websocketpp::config::asio_tls> server_type;

struct ClientList {
    std::string address;
    websocketpp::connection_hdl connection;
    std::list<std::string> clientList;
};

class ServerHost {

    public:
        void StartServer(int, std::list<ServerSocket> *, std::string, std::string);
        void Callback_OnMessage(websocketpp::connection_hdl connection_hdl, server_type::message_ptr message);

    private:
        void OnMessage(websocketpp::connection_hdl, server_type::message_ptr);
        void OnOpen();
        void OnClose(websocketpp::connection_hdl);

        void AddClient(std::string, websocketpp::connection_hdl);
        void SendClientUpdate();

        void AddNewExternalClientList(websocketpp::connection_hdl, std::string);
        void UpdateExternalClientList(websocketpp::connection_hdl, nlohmann::json);
        void SendAllClientLists(websocketpp::connection_hdl);
        void SendAllClientListsToAllClients();

        void SendChatMessage(nlohmann::json, nlohmann::json);
        void SendPublicChatMessage(nlohmann::json);

        bool CheckServerSignature(std::string);

        std::list<std::string> clientList;
        std::list<websocketpp::connection_hdl> clientConnections;

        std::list<ServerSocket> * serverSockets;
        std::list<ClientList> externalClientLists;

        std::string myAddress;
        std::string publicKey;

        server_type server;
};

