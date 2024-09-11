#pragma once
#include "Program.h"
#include <vector>
#include "Font.h"
#include <unordered_map>
#include <ctime>
#include <iomanip>
#include <sstream>
#include "ChatMessage.h"
#include "Client.h"

// Websocket++
#include "ClientSocket.h"
#include <thread>

class ChatApplication : public Program{

    public:
        
        void Start() override;
        void Update() override;
        void End() override;

        void DrawCustomUserButtons(bool& scroll);

        /*
            Populates the fontList vector (configuring each font respectivley)
        */
        void Configure_FontList();

        std::string GetCurrentTime(bool forUser);


        void PushFont(Font fontIndex, ImVec4 colour = ImVec4(0.0,0.0,0.0,0.0));
        void PopFont();



    private:
        
        Client currentClient;

        std::vector<FontData> fontList;

        int counter = 0;

        std::vector<ImVec4> colourVector;
        
        int selectedUser;

        // Socket variables
        ClientSocket socket;
        std::thread socketThread;

       

};