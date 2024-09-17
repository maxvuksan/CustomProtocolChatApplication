#pragma once
#include <string>

struct ActiveUsers{
    
    std::string username;
    std::string mostRecentMessage;

    int colourIndex;


    std::string serverOfOrigin; // where this client is from
    bool marked; // used for temporary marking, specifically when updating the client list

};
