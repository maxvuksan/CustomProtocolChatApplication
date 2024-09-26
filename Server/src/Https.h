#pragma once


#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

#include <asio.hpp>
#include <asio/ssl.hpp>

#include <json.hpp>

using asio::ip::tcp;
using namespace std;
using Json = nlohmann::json;
namespace fs = std::filesystem;

class HttpsSession : public std::enable_shared_from_this<HttpsSession> {
public:
    HttpsSession(tcp::socket socket, asio::ssl::context& ssl_context, string serverIp)
        : socket_(std::move(socket), ssl_context) { ip = serverIp; }

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

                    string request(data_, length);

                    if (request.find("POST /api/upload") != std::string::npos) {
                        mode = "upload";
                    }

                    if (is_file_upload(request) && mode == "upload") {
                        string boundaryPrefix = "Content-Type: multipart/form-data; boundary=";
                        int pos = request.find(boundaryPrefix);
                        pos += boundaryPrefix.length();
                        int endPos = request.find("\r\n", pos);
                        boundary = request.substr(pos, endPos - pos);
                    } else if (mode == "upload") {
                        string fileName;
                        string fileContent;

                        int contentPos = request.find(boundary);
                        contentPos += boundary.length();

                        int fileNamePos = request.find("filename=", contentPos) + 10;
                        int fileNamePosEnd = request.find("\"", fileNamePos);
                        fileName = request.substr(fileNamePos, fileNamePosEnd - fileNamePos);

                        contentPos = request.find("\r\n\r\n", contentPos) + 4;

                        int contentPosEnd = request.find(boundary, contentPos);
                        contentPosEnd -= 3;

                        // Extract the part containing the file
                        fileContent = request.substr(contentPos, contentPosEnd - contentPos);

                        save_file(fileName, fileContent);
                    }

                    do_read();

                    // do_write(length);
                } else {

                    

                    std::cerr << "Read failed: " << error.message() << std::endl;
                }
            });
    }

    void save_file(const std::string& file_name, const std::string& file_content) {
        // Save the file to a directory (ensure the directory exists)
        fs::path path = "uploads"; // Specify your upload directory
        fs::create_directories(path); // Create directory if it doesn't exist
        std::ofstream file(path / file_name, std::ios::binary);

        

        if (file) {
            file.write(file_content.data(), file_content.size());
            std::cout << "File saved: " << (path / file_name) << std::endl;

            asio::error_code ec;
            
            string url;
            url = "https://" + ip + ":443/" + file_name;

            Json jsonResponse;
            jsonResponse["response"]["body"]["file_url"] = url;

            string responseString = jsonResponse.dump();
            string responseHttp = "HTTP/1.1 200 OK\r\n"
                               "Content-Type: application/json\r\n"
                               "Content-Length: " + std::to_string(responseString.size()) + "\r\n"
                               "\r\n" + responseString;

            asio::write(socket_, asio::buffer(responseHttp), ec);

            try {
                socket_.shutdown();
            } catch (const system_error& e) {
                cerr << "Error" << e.what() << endl;
            }



        } else {
            std::cerr << "Failed to save file: " << file_name << std::endl;
        }
    }

    bool is_file_upload(const std::string& request) {
        // Check for the presence of "multipart/form-data" in the request
        return request.find("Content-Type: multipart/form-data") != std::string::npos;
    }

    void parse_file_upload(const std::string& request, std::string& file_name, std::string& file_content) {
        // Find the boundary
    std::string boundary = "--boundary"; // Update this to the actual boundary
    std::string boundary_prefix = boundary + "\r\n";
    
    auto pos = request.find(boundary_prefix);
    if (pos == std::string::npos) {
        std::cerr << "Boundary not found in request" << std::endl;
        return;
    }

    // Move past the boundary prefix
    pos += boundary_prefix.length();

    // Find the end of the file content
    auto end_boundary_pos = request.find(boundary, pos);
    if (end_boundary_pos == std::string::npos) {
        std::cerr << "End boundary not found" << std::endl;
        return;
    }

    // Extract the part containing the file
    std::string file_part = request.substr(pos, end_boundary_pos - pos);

    // Find the content disposition
    std::string content_disposition_prefix = "Content-Disposition: form-data; ";
    auto filename_pos = file_part.find("filename=\"");
    if (filename_pos == std::string::npos) {
        std::cerr << "Filename not found in part" << std::endl;
        return;
    }
    filename_pos += 10; // Move past 'filename="'

    // Find the end of the filename
    auto filename_end_pos = file_part.find("\"", filename_pos);
    if (filename_end_pos == std::string::npos) {
        std::cerr << "End of filename not found" << std::endl;
        return;
    }

    // Extract the filename
    file_name = file_part.substr(filename_pos, filename_end_pos - filename_pos);

    // Find the content type (optional, but good for logging/validation)
    auto content_type_pos = file_part.find("Content-Type: ");
    std::string content_type;
    if (content_type_pos != std::string::npos) {
        content_type_pos += 14; // Move past 'Content-Type: '
        auto content_type_end_pos = file_part.find("\r\n", content_type_pos);
        content_type = file_part.substr(content_type_pos, content_type_end_pos - content_type_pos);
    }

    // Find the content start after the headers
    auto content_start_pos = file_part.find("\r\n\r\n", filename_end_pos) + 4; // Move past '\r\n\r\n'
    if (content_start_pos == std::string::npos) {
        std::cerr << "Content start not found" << std::endl;
        return;
    }

    // Extract the file content
    file_content = file_part.substr(content_start_pos);

    // Optional: Log the filename and content type
    std::cout << "Filename: " << file_name << ", Content-Type: " << content_type << std::endl;

        
    }

    void do_write(std::size_t length) {
        auto self(shared_from_this());
        asio::async_write(socket_, asio::buffer(data_, length),
            [this, self](const asio::error_code& error, std::size_t /*length*/) {
                if (!error) {
                    // do_write();
                } else {
                    std::cerr << "Write failed: " << error.message() << std::endl;
                }
            });
    }

    string boundary;
    string mode = "";
    string ip;

    asio::ssl::stream<tcp::socket> socket_;
    char data_[1024];
    
};

class HttpsServer {
public:
    HttpsServer(asio::io_context& io_context, short port, asio::ssl::context& ssl_context, string serverIp)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
          ssl_context_(ssl_context) {
        ip = serverIp;
        do_accept();
    }

    void StartServer() {
        
    }

private:
    void do_accept() {
        acceptor_.async_accept(
            [this](const asio::error_code& error, tcp::socket socket) {
                if (!error) {
                    std::make_shared<HttpsSession>(std::move(socket), ssl_context_, ip)->start();
                }
                do_accept();
            });
    }

    string ip;

    asio::io_context io_context;
    tcp::acceptor acceptor_;
    asio::ssl::context& ssl_context_;
};

class Https {
    public:
        Https(string);
        void StartServer();

    private:
        HttpsServer * httpsServer;
        asio::io_context io_context;
        asio::ssl::context * ssl_context;

        std::string ip;

};