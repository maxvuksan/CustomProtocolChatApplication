#pragma once

#include <thread>
#include <sstream>
#include <list>
#include <fstream>
#include <mutex>

#include "ServerSocket.h"
#include "ServerHost.h"
#include "Https.h"
#include "Encryption.h"

#include <asio.hpp>
#include <asio/ssl.hpp>

class Server {

    public:
        int StartServer();
        void CommandManager(std::string);

        int ConnectToServer(std::string);

        ~Server();

    private:
        void RemoveSocketAt(int);
        void CheckSocketStatus();
        void GetRSAKeys(std::string, int);

        ServerHost serverHost;
        std::thread hostThread;

        Https * httpsServer;
        std::thread httpsThread;

        asio::io_context io_context;

        std::list<ServerSocket> socketList;
        std::list<std::thread> threadList;
        std::list<std::string> addressList;

        std::string ip;
        std::string address;

        std::string privateKey;
        std::string publicKey;

        Encryption encryption;
};