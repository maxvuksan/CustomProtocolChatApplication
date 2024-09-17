#pragma once

#include <thread>
#include <sstream>
#include <list>

#include "ServerSocket.h"
#include "ServerHost.h"


class Server {

    public:
        int StartServer();
        void CommandManager(std::string);

        int ConnectToServer(std::string);

        ~Server();

    private:
        ServerHost serverHost;
        std::thread hostThread;

        std::list<ServerSocket> socketList;
        std::list<std::thread> threadList;
        std::list<std::string> addressList;

        std::string address;
};