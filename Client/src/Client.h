#pragma once

#include "ChatMessage.h"
#include "ActiveUsers.h"
#include <vector> 
#include <unordered_map>


class Client {

    

    public:
        Client();
        const std::unordered_map<std::string, std::vector<ChatMessage>>& GetAllMessages();
        const std::vector<ChatMessage>& GetUserMessages(std::string users); 
        const std::vector<ActiveUsers>& GetActiveUsers();
        const ChatMessage& GetChatMessage(std::string user, int index);

        void UserLeave(std::string user);
        void UserJoin(std::string user);
        void PushMessage(ChatMessage message, std::string user);
        int UpdateDate(std::string user, std::string date, std::string selectedUser);
        int GetColourIndex(std::string user);

    private:
        void ParseMessage(std::string currentUser, ChatMessage currentMessage);
        

        auto compareByDate(const ActiveUsers& a, const ActiveUsers& b);

        std::unordered_map<std::string, std::vector<ChatMessage>> allMessages;

        std::vector<ActiveUsers> activeUsers;
};