#pragma once
#include "Program.h"
#include <vector>
#include "Font.h"


struct ChatMessage{
    std::string message;
    std::string date = "08/16/2024 12:32 PM";
};





class ChatApplication : public Program{

    public:
        
        void Start() override;
        void Update() override;

        /*
            Populates the fontList vector (configuring each font respectivley)
        */
        void Configure_FontList();


        void PushFont(Font font_index);
        void PopFont();



    private:

        char chatBuffer[100];

        std::vector<FontData> fontList;

        std::vector<ChatMessage> messages;

        std::vector<std::string> users;
        int selectedUser;

};