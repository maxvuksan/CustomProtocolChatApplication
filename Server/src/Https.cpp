#include "Https.h"

using namespace std;

Https::Https(string serverIp) {
    
    ssl_context = new asio::ssl::context(asio::ssl::context::sslv23);
    ssl_context->use_certificate_chain_file("-server.crt");
    ssl_context->use_private_key_file("server.key", asio::ssl::context::pem);

    ip = serverIp;

    httpsServer = new HttpsServer(io_context, std::atoi("443"), *ssl_context, ip);

    
}

void Https::StartServer() {
    io_context.run();
}
