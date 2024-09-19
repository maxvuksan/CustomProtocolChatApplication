#include "Client.h"
#include <algorithm>
#include "ChatApplication.h"

Client::Client(){
    allMessages.clear();
    activeUsers.clear();

    activeUsers = {
            {"lexi", "", 1},
            {"11", "", 2},
            {"10", "", 3},
            {"5", "", 4},
            {"6", "", 2},
            {"7", "", 4},
            {"8", "", 3},
    };

    std::vector<ChatMessage> messages1 = {
        ChatMessage({"Hey guys how is everyone"}),

    };

    allMessages["lexi"].push_back(ChatMessage({"Hey guys how is everyone", "123", "22/08/2024 1:32 PM"}));
    allMessages["11"].push_back(ChatMessage({"Hello my name is ashlan", "ash", "22/08/2024 1:32 PM"}));
    allMessages["10"].push_back(ChatMessage({"cat cat cat cat cat cat cat cat", "max", "22/08/2024 1:32 PM"}));
    allMessages["5"].push_back(ChatMessage({"johnny time", "john star", "22/08/2024 1:32 PM"}));

}

void Client::RemoveMarkedClients(){

    bool markedFound = true;
    
    while(markedFound){
        
        markedFound = false;

        for(int i = 0; i < activeUsers.size(); i++){

            // we found something to remove
            if(activeUsers[i].marked){
                markedFound = true;
                activeUsers.erase(activeUsers.begin() + i);
                break;
            } 
        }
    }
}

void Client::MarkClient(int index, bool marked){
    activeUsers[index].marked = marked;
}

void Client::ParseMessage(std::string currentUser, ChatMessage currentMessage){
    allMessages[currentUser].push_back(currentMessage);
    return;
}

const std::unordered_map<std::string, std::vector<ChatMessage>>& Client::GetAllMessages(){
    return allMessages;
}

auto Client::compareByDate(const ActiveUsers& a, const ActiveUsers& b) {
    return a.mostRecentMessage > b.mostRecentMessage;
};


const ChatMessage& Client::GetChatMessage(std::string currentUser, int index){
    return allMessages[currentUser][index];
}

void Client::PushActiveUser(std::string username, std::string serverOfOrigin, bool marked)
{   
    this->activeUsers.push_back(ActiveUsers({username, "0", ChatApplication::GetRandomColourIndex(), serverOfOrigin, marked}));
}

int Client::GetColourIndex(std::string user){

    for(int i = 0; i < activeUsers.size(); i++){

        if(activeUsers[i].username == user){
            return activeUsers[i].colourIndex;
        }

    }

    return 0;
}

const std::vector<ChatMessage>& Client::GetUserMessages(std::string users){
    // will probably need to do a user check later
    return allMessages[users];
}

const std::vector<ActiveUsers>& Client::GetActiveUsers(){
    return activeUsers;
}

int Client::GetClientIndex(std::string username, std::string serverOfOrigin){

    for(int i = 0; i < activeUsers.size(); i++){
        if(activeUsers[i].username == username && activeUsers[i].serverOfOrigin == serverOfOrigin){
            return i;
        }
    }
    return -1;
}


void Client::PushMessage(ChatMessage currentMessage, std::string currentUser){
    allMessages[currentUser].push_back(currentMessage);

    return;
}

int Client::UpdateDate(std::string user, std::string date, std::string selectedUser){

    for(int i = 0; i < activeUsers.size(); i++){
        if(activeUsers[i].username == user){
            activeUsers[i].mostRecentMessage = date;
        }
    }

    std::sort(activeUsers.begin(), activeUsers.end(),
        [this](const ActiveUsers& a, const ActiveUsers& b) {
            return this->compareByDate(a, b);
        });

    int index = 0;

    for(int i = 0; i < activeUsers.size(); i++){
        if(activeUsers[i].username == selectedUser){
            index = i;
        }
    }

    return index;
}


void Client::UserLeave(std::string user){
    // remove user from 'activeUsers' and 'allMessages' map
    allMessages.erase(user);

    for(int i = 0; i < activeUsers.size(); i++){
        
        if(activeUsers[i].username == user){
            activeUsers.erase (activeUsers.begin()+i);
        }
    }

    return;
}

void Client::UserJoin(std::string user){
    // add user to 'activeUsers' and initialise in map
    activeUsers.push_back({user, ""});
    allMessages[user] = {};
    return;
}