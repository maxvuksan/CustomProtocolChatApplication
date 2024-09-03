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

        std::list<ServerSocket> serverSockets;
        std::list<std::thread> socketThreads;

        int count = 0;
        ServerSocket s1;
        ServerSocket s2;

        std::thread t1;
        std::thread t2;

        
        
};