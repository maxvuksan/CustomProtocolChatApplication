#include "Https.h"

using namespace std;

Https::Https(string serverIp, int port) {
    
    ssl_context = new asio::ssl::context(asio::ssl::context::sslv23);
    ssl_context->use_certificate_chain_file("-server.crt");
    ssl_context->use_private_key_file("server.key", asio::ssl::context::pem);

    ip = serverIp;

    httpsServer = new HttpsServer(io_context, port, *ssl_context, ip);

    
}

void Https::StartServer() {
    io_context.run();
}
