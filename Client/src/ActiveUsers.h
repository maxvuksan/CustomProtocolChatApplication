#pragma once
#include <string>

struct ActiveUsers{
    
    // public key
    std::string publicKey;
    std::string pseudoName;
    std::string fingerprint;
    std::string mostRecentMessage;

    int colourIndex;


    std::string serverOfOrigin; // where this client is from
    bool marked; // used for temporary marking, specifically when updating the client list

};
