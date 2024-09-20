#pragma once

#include <thread>
#include <sstream>
#include <list>
#include <fstream>
#include <mutex>

#include <asio.hpp>
#include <asio/ssl.hpp>

#include "ServerSocket.h"
#include "ServerHost.h"

using asio::ip::tcp;

class HttpsSession : public std::enable_shared_from_this<HttpsSession> {
public:
    HttpsSession(tcp::socket socket, asio::ssl::context& ssl_context)
        : socket_(std::move(socket), ssl_context) {}

    void start() {
        do_handshake();
    }

private:
    void do_handshake() {
        auto self(shared_from_this());
        socket_.async_handshake(asio::ssl::stream_base::server,
            [this, self](const asio::error_code& error) {
                if (!error) {
                    do_read();
                } else {
                    std::cerr << "Handshake failed: " << error.message() << std::endl;
                }
            });
    }

    void do_read() {
        auto self(shared_from_this());
        socket_.async_read_some(asio::buffer(data_),
            [this, self](const asio::error_code& error, std::size_t length) {
                if (!error) {
                    do_write(length);
                } else {
                    std::cerr << "Read failed: " << error.message() << std::endl;
                }
            });
    }

    void do_write(std::size_t length) {
        auto self(shared_from_this());
        asio::async_write(socket_, asio::buffer(data_, length),
            [this, self](const asio::error_code& error, std::size_t /*length*/) {
                if (!error) {
                    do_read();
                } else {
                    std::cerr << "Write failed: " << error.message() << std::endl;
                }
            });
    }

    asio::ssl::stream<tcp::socket> socket_;
    char data_[1024];
};

class HttpsServer {
public:
    HttpsServer(asio::io_context& io_context, short port, asio::ssl::context& ssl_context)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
          ssl_context_(ssl_context) {
        do_accept();
    }

private:
    void do_accept() {
        acceptor_.async_accept(
            [this](const asio::error_code& error, tcp::socket socket) {
                if (!error) {
                    std::make_shared<HttpsSession>(std::move(socket), ssl_context_)->start();
                }
                do_accept();
            });
    }

    tcp::acceptor acceptor_;
    asio::ssl::context& ssl_context_;
};


class Server {

    public:
        int StartServer();
        void CommandManager(std::string);

        int ConnectToServer(std::string);

        ~Server();

    private:
        void RemoveSocketAt(int);
        void CheckSocketStatus();
        void StartHttpsServer();

        ServerHost serverHost;
        std::thread hostThread;

        std::list<ServerSocket> socketList;
        std::list<std::thread> threadList;
        std::list<std::string> addressList;

        std::string address;
};