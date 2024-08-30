#pragma once

#include "ChatMessage.h"
#include <thread>

class ClientSocket {
    public:
        void Start();
        void End();

        int SendChatMessage(ChatMessage chatMessage);
    private:
        

};