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
        
        ServerSocket serverSocket1;
        std::thread socketThread1;  

        ServerSocket serverSocket2;
        std::thread socketThread2;       
};