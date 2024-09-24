#pragma once
#include <sstream>

struct ChatMessage{
    std::string message;
    std::string sentBy;
    std::string date;

    bool isFile = false;
    std::string filename = "ExampleFile.txt";
    std::string fileURL = "https://www.example.com";

};

