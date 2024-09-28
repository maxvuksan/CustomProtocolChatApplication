#pragma once

#include "ChatMessage.h"
#include "ActiveUsers.h"
#include <vector> 
#include <unordered_map>

#include <cryptopp/aes.h>

class Client {

    public:

        Client();

        const std::unordered_map<std::string, std::vector<ChatMessage>>& GetAllMessages();

        const std::vector<ChatMessage>& GetUserMessages(std::string users); 

        // removes all active clients with the marked flag set to true
        void RemoveMarkedClients();
        void MarkClient(int index, bool marked);

        /*
            @returns a referenced to the active user vector
        */
        const std::vector<ActiveUsers>& GetActiveUsers();

        /*
            @returns index of client, -1 if no client is found
        */        
        int GetClientIndex(std::string username);

        /*
            get a specific chat message from a given user
        */
        const ChatMessage& GetChatMessage(std::string username, int index);

        void ClearActiveUsers(){activeUsers.clear();}

        /*
            @param username display name for the client (will generally be the public key)
            @param serverOfOrigin where is this client from? formatted "ip:port"
        */
        void PushActiveUser(std::string username, std::string serverOfOrigin, bool marked = false);

        void UserLeave(std::string user);
        void UserJoin(std::string user);
        void PushMessage(ChatMessage message, std::string user);
        int UpdateDate(std::string user, std::string date, std::string selectedUser);
        int GetColourIndex(std::string user);
        std::string GetKeyFromFingerprint(std::string fingerprint);
        std::string GetPseudoNameFromFingerprint(std::string fingerprint);

    private:
        void ParseMessage(std::string currentUser, ChatMessage currentMessage);

        auto compareByDate(const ActiveUsers& a, const ActiveUsers& b);

        std::unordered_map<std::string, std::vector<ChatMessage>> allMessages;

        std::vector<ActiveUsers> activeUsers;
};