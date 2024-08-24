#include "ChatApplication.h"
#include "Client.h"


void ChatApplication::Start(){
    // Creating thread for client socket
    // Currently not being joined anywhere
    socketThread = std::thread(&ClientSocket::Start, &socket); 

    Configure_FontList();
    counter = 0;
    selectedUser = 0;
}

void ChatApplication::Configure_FontList(){

    // FONT_PRIMARY
    FontData primaryFont;
    primaryFont.path = "Client/assets/fonts/gg sans Regular.ttf";
    primaryFont.colour = ImVec4(1.0,1.0,1.0,1.0);
    primaryFont.characterSize = 24;

    // FONT_SECONDARY
    FontData secondaryFont;
    secondaryFont.path = "Client/assets/fonts/gg sans Regular.ttf";
    secondaryFont.colour = ImVec4(1.0,1.0,1.0,0.7);
    secondaryFont.characterSize = 20;

    // FONT_BOLD
    FontData boldFont;
    boldFont.path = "Client/assets/fonts/gg sans Bold.ttf";
    boldFont.colour = ImVec4(1.0,1.0,1.0,1.0);
    boldFont.characterSize = 20;


    fontList.push_back(primaryFont);
    fontList.push_back(secondaryFont);
    fontList.push_back(boldFont);

    ImGuiIO& io = ImGui::GetIO();

    colourVector = {
        {0.16, 0.16, 0.18, 1.0}, // blackish
        {0.42, 0.34, 0.44, 1.0}, // purple but not vibrant
        {0.85, 0.78, 0.75, 1.0}, // grey beige
        {0.98, 0.51, 0.52, 1.0}, // pink/salmon
        {0.69, 0.66, 0.89, 1.0}, // pastel blue
        {1.0, 1.0, 1.0, 1.0}, // white
    };

    for(int i = 0; i < fontList.size(); i++){

        
        fontList[i].imguiFontRef = io.Fonts->AddFontFromFileTTF(fontList[i].path.c_str(), fontList[i].characterSize); 

        if (fontList[i].imguiFontRef == nullptr) {
            std::cerr << "Failed to load font: " << fontList[i].path << std::endl;
        }


        if(i == 0){
            // set font at index 0 as the default
            io.FontDefault = io.Fonts->Fonts.back(); 
        }
    }

}

void ChatApplication::PushFont(Font fontIndex, ImVec4 colour){
    ImGui::PushFont(fontList[fontIndex].imguiFontRef); // font

    if(colour.w == 0.0){
        colour = fontList[fontIndex].colour;
    }

    ImGui::PushStyleColor(ImGuiCol_Text, colour);// font colour
}
void ChatApplication::PopFont(){
    ImGui::PopFont();
    ImGui::PopStyleColor(1);
}

std::string ChatApplication::GetCurrentTime(bool forUser){
    // Get current time
    time_t now = time(0);
    tm* localtm = localtime(&now);
    
    // Create a string stream to format the time
    std::ostringstream oss;
    
    // Format the time as 22/08/24 1:19 PM

    if(forUser){
        oss << std::put_time(localtm, "%d/%m/%y %I:%M:%S %p");
    } else {
        oss << std::put_time(localtm, "%d/%m/%y %I:%M %p");
    }
    
    
    return oss.str();
}


void ChatApplication::Update(){
    static char inputBuffer[256] = "";  // Buffer to hold the input text
    static bool scroll = true;
    
    // Max's example ImGui window! ----------------------------------------------------------------------

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
    ImGui::Begin("Example Window", nullptr); // Create a new ImGui window
    
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0)); // Remove spacing between items
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0); // Remove padding inside the button
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10,10));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);

        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.70f, 0.70f, 0.70f, 0.65f)); // Red border
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.09f, 0.09f, 0.15f, 1.00f)); 


        // Get the window size and set the button width to match it
        ImVec2 windowSize = ImGui::GetWindowSize();
        float buttonWidth = windowSize.x - ImGui::GetStyle().WindowPadding.x * 2.0f; // Adjust for padding

        for(int i = 0; i < currentClient.GetActiveUsers().size(); i++){

            if(i == selectedUser){
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.45f, 0.45f, 0.55f, 1.00f)); 
            }

            int newSelectedUser = selectedUser;
            if (ImGui::Button(currentClient.GetActiveUsers()[i].username.c_str(), ImVec2(buttonWidth, 0))){ // Add a button
                newSelectedUser = i;
                scroll = true;
            }

            if(i == selectedUser){
                ImGui::PopStyleColor(1); 
            }

            selectedUser = newSelectedUser;

        }

        ImGui::PopStyleVar(4); 
        ImGui::PopStyleColor(2);

    ImGui::End(); // End the ImGui window
    ImGui::PopStyleVar(1);


    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(25, 25));
    ImGui::Begin("Messages", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse); 
    ImGui::PopStyleVar(1);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,25));

    ImVec2 contentRegion = ImGui::GetContentRegionAvail();
    float inputBoxHeight = 30.0f; // Adjust this value according to your input box height
    ImVec2 scrollAreaSize = ImVec2(contentRegion.x, contentRegion.y - 40); // Reserve space for the input box

    ImGui::BeginChild("MessagesScrollArea", scrollAreaSize, true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
    
    auto messages = currentClient.GetAllMessages();

    for(int i = 0; i < messages[currentClient.GetActiveUsers()[selectedUser].username].size(); i++){
        
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,5));
        
        if(currentClient.GetChatMessage(currentClient.GetActiveUsers()[selectedUser].username, i).sentBy == "me"){
            PushFont(FONT_BOLD, colourVector[5]);
        } else {
            PushFont(FONT_BOLD, colourVector[currentClient.GetColourIndex(currentClient.GetChatMessage(currentClient.GetActiveUsers()[selectedUser].username, i).sentBy)]);
        }

        ImGui::Text(currentClient.GetChatMessage(currentClient.GetActiveUsers()[selectedUser].username, i).sentBy.c_str());
        ImGui::SameLine();
        ImGui::Text("   ");
        ImGui::SameLine();
        PopFont();

        // draw date
        PushFont(FONT_SECONDARY);
        ImGui::Text(currentClient.GetChatMessage(currentClient.GetActiveUsers()[selectedUser].username, i).date.c_str());
        PopFont();

        ImGui::PopStyleVar(1);
        
        PushFont(FONT_PRIMARY);
        ImGui::Text(currentClient.GetChatMessage(currentClient.GetActiveUsers()[selectedUser].username, i).message.c_str());
        PopFont();
        
    }

    if(scroll){
        ImGui::SetScrollHereY(1.0f);
        scroll = false;
    }

    

    ImGui::EndChild(); // End the scrollable area

    ImGui::PopStyleVar(1);

    // Add a separator and a text box at the bottom of the window
    ImGui::Separator();

    ImGui::SetNextItemWidth(contentRegion.x);

    if (ImGui::InputText("##MessageInput", inputBuffer, IM_ARRAYSIZE(inputBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {

        // Process the input here, for example, send the message
        std::string newMessage(inputBuffer);
        if (!newMessage.empty()) {
            // Add the new message to the selected user's message list (pseudo-code)
            currentClient.PushMessage({newMessage, "me", GetCurrentTime(false)}, currentClient.GetActiveUsers()[selectedUser].username);

            selectedUser = currentClient.UpdateDate(currentClient.GetActiveUsers()[selectedUser].username, GetCurrentTime(true), currentClient.GetActiveUsers()[selectedUser].username);

            // Clear the input buffer
            inputBuffer[0] = '\0';
            ImGui::SetKeyboardFocusHere(-1);
            scroll = true;

            counter++;
            if(counter == 5){
                currentClient.UserLeave("lexi");
                currentClient.UserJoin("luke");
            }
        }
    } 

    ImGui::End();
}

void ChatApplication::End() {
    socketThread.join();
}
/// Websocket++

