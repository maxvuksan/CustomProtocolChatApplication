#pragma once

#include <thread>

#include "ServerSocket.h"
#include "ServerHost.h"

class Server {

    public:
        int StartServer();
        ~Server();

    private:
        ServerHost serverHost;
        std::thread hostThread;
};