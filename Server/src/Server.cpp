#include "Server.h"

using namespace std;

int Server::StartServer() {




    cout << "Enter server public ip " << endl;
    cin >> address; 
    int port;
    cout << "Enter server port " << endl;
    cin >> port;

    address += ":" + to_string(port);

    cin.ignore(numeric_limits<std::streamsize>::max(), '\n');
    
    hostThread = thread(&ServerHost::StartServer, &serverHost, port, &socketList, address);

    // Connect to servers in server list.txt
    ifstream file("Server List.txt");
    if (!file.is_open()) {
        return -1; 
    }

    string line;
    while (getline(file, line)) {
        // ConnectToServer(line);
    }

    file.close();




    while (1) {
        string command;
        getline(cin, command);

        CommandManager(command);
    }

    return 0;
}

void Server::CommandManager(string command) {
    istringstream iss(command);

    string word;
    string sections[5];
    
    int counter = 0;
    while (iss >> word && counter < 5) {
        sections[counter] = word;
        counter++;
    }

    // Commands
    int er = -2;
    if (sections[0] == "connect") {
        er = ConnectToServer(sections[1]);
    } 

    switch (er) {
        case -2:
            cout << "Unknown command" << endl;
            break;
    }
}

int Server::ConnectToServer(string dstIp) {
    
    socketList.emplace_back();
    addressList.push_back(dstIp);

    ServerSocket & lastSocket = socketList.back(); 
    threadList.emplace_back(&ServerSocket::ConnectToServer, socketList.rbegin(), dstIp, address);
    
    return 0;
}

Server::~Server() {
    if (hostThread.joinable()) {
        hostThread.join();
    }
}