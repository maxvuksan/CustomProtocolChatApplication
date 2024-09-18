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

enum ConnectedState{
    
    CS_DISCONNECTED,
    CS_IN_PROGRESS,
    CS_CONNECTED,
};

class ChatApplication : public Program{

    public:
        
        void Start() override;
        void Update() override;
        void End() override;

        void DrawConnectToServerModal();
        void DrawCustomUserButtons(bool& scroll);

        /*
            Populates the fontList vector (configuring each font respectivley)
        */
        void Configure_FontList();
        static void Configure_PseudoNameList();

        std::string GetCurrentDateTime(bool forUser);

        void PushFont(Font fontIndex, ImVec4 colour = ImVec4(0.0,0.0,0.0,0.0));
        void PopFont();

        void SetConnectedState(ConnectedState state){this->connectedState = state;}

        /*
            @returns the index of a random colour
        */
        static int GetRandomColourIndex(){return rand() % colourVector.size();}

        static std::string GetPsuedoNameFromInt(int index){return pseudoNameVector[index % pseudoNameVector.size()];}

    private:

        Client currentClient;

        std::vector<FontData> fontList;

        ConnectedState connectedState;
        std::string serverAddressToJoin;

        static std::vector<std::string> pseudoNameVector; // display names
        static std::vector<ImVec4> colourVector;
        static std::vector<ImVec4> colourVectorU32;
        
        int selectedUser;

        // Socket variables
        ClientSocket socket;
        std::thread socketThread;

       

};