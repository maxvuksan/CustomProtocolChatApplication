#include "ServerSocket.h"

using namespace std;

typedef websocketpp::client<websocketpp::config::asio_client> client;

int ServerSocket::ConnectToServer(string ip) {
    cout << "Attempting to connect to server with ip: " << ip << endl;

    return 0;
}