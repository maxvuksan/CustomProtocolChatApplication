#include "HttpsServer.h"

using namespace std;

void HttpsServer::StartServer() {
    try {
        asio::io_context io_context;

        asio::ssl::context ssl_context(asio::ssl::context::sslv23);
        ssl_context.use_certificate_chain_file("-server.crt");
        ssl_context.use_private_key_file("server.key", asio::ssl::context::pem);

        HttpsServer server(io_context, std::atoi("443"), ssl_context);

        io_context.run();
        } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

