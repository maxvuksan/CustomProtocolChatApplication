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
            @param publicKey 
            @param serverOfOrigin where is this client from? formatted "ip:port"
        */
        void PushActiveUser(std::string publicKey, std::string serverOfOrigin, bool marked = false);

        void PushMessage(ChatMessage message, std::string user);
        int UpdateDate(std::string user, std::string date, std::string selectedUser);
        int GetColourIndex(std::string user);
        std::string GetKeyFromFingerprint(std::string fingerprint);
        std::string GetPseudoNameFromFingerprint(std::string fingerprint);
        std::string GetFingerprintFromKey(std::string key);
        std::string GetPseudoNameFromKey(std::string key);
        std::string GetKeyFromPseudoName(std::string pseudoName);

    private:
        void ParseMessage(std::string currentUser, ChatMessage currentMessage);

        auto compareByDate(const ActiveUsers& a, const ActiveUsers& b);

        std::unordered_map<std::string, std::vector<ChatMessage>> allMessages;

        std::vector<ActiveUsers> activeUsers;
};