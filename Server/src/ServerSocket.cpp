#include "ServerSocket.h"

using Json = nlohmann::json; 


using namespace std;

void ServerSocket::OnMessage(websocketpp::connection_hdl hdl, websocketpp::config::asio_client::message_type::ptr payload) {
    cout << "Recieved message" << endl;
}

void ServerSocket::OnOpen(websocketpp::connection_hdl hdl) {

    connected = true;
    cout << "connection opened" << endl;

    connection = hdl;

    Json jsonHello;

    jsonHello["type"] = "signed_data";
    jsonHello["data"]["type"] = "server_hello";
    jsonHello["data"]["sender"] = hostAddress;
    jsonHello["counter"] = counter;

    vector<unsigned char> fingerprintVector;
    if (!encryptor.CreateFingerprint(publicKey, fingerprintVector)) {
        cerr << "Fingerprint failed" << endl;
    }

    string fingerprint = mine::Base64::encode(encryptor.VectorToString(fingerprintVector));

    jsonHello["signature"] = fingerprint;

    counter++;

    SendJson(jsonHello);

    Json jsonClientRequest;

    jsonClientRequest["type"] = "client_update_request";

    SendJson(jsonClientRequest);
}

void ServerSocket::OnFail(websocketpp::connection_hdl hdl) {
    usleep(10000000);
    ConnectToServer(connectionAddress, hostAddress, publicKey);
}

void ServerSocket::OnClose(websocketpp::connection_hdl hdl) {
    connected = false;

}

void ServerSocket::Stop() {
    connected = false;
}

ServerSocket::~ServerSocket() {
    cout << "Cleaning up" << endl;
        
    
}

ServerSocket::ServerSocket() {
    connected = false;

    client.set_access_channels(websocketpp::log::alevel::all); // Changed from all
    client.clear_access_channels(websocketpp::log::alevel::frame_payload);

    client.init_asio();

    client.set_message_handler(bind(&ServerSocket::OnMessage, this, placeholders::_1, placeholders::_2));
    client.set_open_handler(bind(&ServerSocket::OnOpen, this, placeholders::_1));
    client.set_fail_handler(bind(&ServerSocket::OnFail, this, placeholders::_1));
    client.set_close_handler(bind(&ServerSocket::OnClose, this, placeholders::_1));

}

void ServerSocket::ConnectToServer(string dstIp, string srcAddress, string key) {

    if (hostAddress == "") {
        hostAddress = srcAddress;
        publicKey = key;
        connectionAddress = dstIp;
    }

    websocketpp::lib::error_code ec; 
    
    cout << "Attempting to connect to server with ip: " << connectionAddress << endl;

    string address = "wss://" + connectionAddress;

    // Set TLS init handler
    client.set_tls_init_handler([](websocketpp::connection_hdl) -> std::shared_ptr<asio::ssl::context> {
        std::shared_ptr<asio::ssl::context> ctx = std::make_shared<asio::ssl::context>(asio::ssl::context::tlsv12);

        try {
            
            ctx->set_verify_mode(asio::ssl::verify_none);  // Disable verification for certificates (we are using self signed certificates)
            
        } catch (std::exception& e) {
            std::cerr << "Error setting up TLS: " << e.what() << std::endl;
        }

        return ctx;
    });

    Client::connection_ptr con = client.get_connection(address, ec);
    connection = con->get_handle();

    if (ec) {  
        cout << "Connection attempt failed: " << ec.message() << endl;
        
        return;
    }

    client.connect(con);
    client.run();
    
    return;
}

bool ServerSocket::IsConnected() {
    return connected;
}

void ServerSocket::SendJson(Json json) {
    if (client.get_con_from_hdl(connection)->get_state() != websocketpp::session::state::open) {
        cerr << "Connection not open" << endl;
    }

    client.send(connection, json.dump(), websocketpp::frame::opcode::text);
}

string ServerSocket::GetConnectionAddress() {
    return connectionAddress;
}