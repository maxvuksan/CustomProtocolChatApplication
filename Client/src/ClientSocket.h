#pragma once

#include "ChatMessage.h"
#include <thread>

class ClientSocket {
    public:
        void Start(std::string finalAddress);
        void End();

        int SendChatMessage(ChatMessage chatMessage);
    private:
        

};