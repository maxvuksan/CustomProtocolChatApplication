#include "Server.h"

using namespace std;

int Server::StartServer() {

    hostThread = thread(&ServerHost::StartServer, &serverHost);

    return 0;
}

Server::~Server() {
    if (hostThread.joinable()) {
        hostThread.join();
    }
}