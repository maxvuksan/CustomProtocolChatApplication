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

    // try {
    //     asio::io_context ioContext;
    //     asio::ssl::context sslContext(asio::ssl::context::sslv23);

    //     sslContext.use_certificate_chain_file("-server.crt");
    //     sslContext.use_private_key_file("server.key", asio::ssl::context::pem);

    //     tcp::acceptor acceptor(ioContext, tcp::endpoint(tcp::v4(), 443));

    //     while (true) {
    //         tcp::socket socket(ioContext);
    //         acceptor.accept(socket);
    //         asio::ssl::stream<tcp::socket> sslSocket(move(socket), sslSocket);
    //     }

    // } catch (exception& e) {
    //     cerr << e.what() << endl;
    // }
    
    
}
