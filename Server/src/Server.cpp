#include "Server.h"

using namespace std;

int Server::StartServer() {

    ifstream file("Server Properties.txt");
    if (!file.is_open()) {
        return -1;
    }

    string line;
    while (getline(file, line)) {
        address = line;
        ip = line;
    }
    file.close();

    if (address == "") {
        cout << "Enter server public ip " << endl;
        cin >> address; 
    }

    int port;
    cout << "Enter server port " << endl;
    cin >> port;

    address += ":" + to_string(port);

    cin.ignore(numeric_limits<std::streamsize>::max(), '\n');
    
    hostThread = thread(&ServerHost::StartServer, &serverHost, port, &socketList, address);

    httpsServer = new Https(ip);
    httpsThread = thread(&Https::StartServer, httpsServer);


    // Connect to servers in server list.txt
    file = ifstream("Server List.txt");
    if (!file.is_open()) {
        return -1; 
    }

    while (getline(file, line)) {
        ConnectToServer(line);
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
        case -3:
            cout << "Cannot connect to your own server" << endl;
            break;
    }
}

// Deprecated
void Server::RemoveSocketAt(int index) {

    auto socketIt = socketList.begin();
    advance(socketIt, index);
    socketList.erase(socketIt);

    auto addressIt = addressList.begin();
    advance(addressIt, index);
    addressList.erase(addressIt);

    auto threadIt = threadList.begin();
    advance(threadIt, index);
    threadIt->join();
    threadList.erase(threadIt);    
}

int Server::ConnectToServer(string dstIp) {
    if (dstIp == address) {
        return -3;
    }

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