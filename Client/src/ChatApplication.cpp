#include "ChatApplication.h"
#include "Client.h"
#include "Globals.h"
#include <fstream>
#include <string>

std::vector<ImVec4> ChatApplication::colourVector = {
    {0.97, 0.96, 0.42, 1.0}, // yellow
    {0.91, 0.47, 0.82, 1.0}, // pink
    {0.59, 0.47, 0.91, 1.0}, // purple
    {0.47, 0.91, 0.91, 1.0}, // blue
    {0.57, 0.91, 0.47, 1.0}, // green
    {1.0, 1.0, 1.0, 1.0}, // white
};

std::vector<std::string> ChatApplication::pseudoNameVector = {};

std::vector<ImVec4> ChatApplication::colourVectorU32 = {
    {247, 245, 108, 255}, // yellow
    {233, 120, 210, 255}, // pink
    {150, 120, 233, 255}, // purple
    {120, 233, 232, 255}, // blue
    {146, 233, 120, 255}, // green
};


void ChatApplication::Start(){
    // Creating thread for client socket

    Configure_PseudoNameList();
    Configure_FontList();
    selectedUser = 0;
    currentPseudoName = "PESUDO NAME NOT SET YET"; // GetPsuedoName();
    connectedState = CS_DISCONNECTED;

    socket.SetChatApplication(this);
    socket.SetClient(&currentClient);

}

void ChatApplication::Configure_PseudoNameList(){

    std::ifstream file("./UsernameList.txt");
    
    if (!file.is_open()) {
        std::cerr << "Error opening file UsernameList.txt" << std::endl;
        return;
    }

    std::string name;
    while (std::getline(file, name)) {
        if (!name.empty()) { // Check if the line is not empty
            pseudoNameVector.push_back(name);
        }
    }

    file.close();

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

std::string ChatApplication::GetPsuedoNameExtractedFromKey(std::string publicKey){

    // assuming grabbing indicies wont break the key 

    int sum = 0;

    // picks 10 characters from the middle of the public key 
    for(int i = 100; i < 110; i++){

        if(i < publicKey.length()){
            sum += (int)publicKey[i];
        }
        else{
            std::cout << "Provided public key is an invalid length, ChatApplication::GetPseudoNameExtractedFromKey\n";
            break;
        }
    }   

    return pseudoNameVector[sum % pseudoNameVector.size()];
}

int ChatApplication::GetRandomColourIndexExtractedFromKey(std::string publicKey){

    int sum = 0;

    for(int i = 100; i < 110; i++){

        if(i < publicKey.length()){
            sum += (int)publicKey[i];
        }
        else{
            std::cout << "Provided public key is an invalid length, ChatApplication::GetRandomColourIndexExtractedFromKey\n";
            break;
        }
    }   

    return sum % colourVectorU32.size();
}


std::string ChatApplication::GetCurrentDateTime(bool forUser){
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


void ChatApplication::DrawCustomUserButtons(bool& scroll){

    if(connectedState != CS_CONNECTED){
        return;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0)); // Remove spacing between items
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0); // Remove padding inside the button
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10,10));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);

    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.70f, 0.70f, 0.70f, 0.65f)); // Red border
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.09f, 0.09f, 0.15f, 1.00f)); 


    // Get the window size and set the button width to match it
    ImVec2 windowSize = ImGui::GetWindowSize();

    ImVec2 childSize = ImVec2(windowSize.x, (currentClient.GetActiveUsers().size() + 1) * (GLOBAL_PADDING + 10));


    // only have scroll area if required
    if(childSize.y > windowSize.y){
        ImGui::BeginChild("ScrollingChild", childSize, true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
    }


    for(int i = 0; i < currentClient.GetActiveUsers().size(); i++){
        if(currentClient.GetActiveUsers()[i].publicKey == socket.publicKey){
            continue;
        }

        ImGuiIO& io = ImGui::GetIO();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 button_size = ImVec2(windowSize.x - ImGui::GetStyle().WindowPadding.x * 2.0f, GLOBAL_PADDING + 10); // Adjust size as needed
        ImVec2 button_pos(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y + GLOBAL_PADDING);
        button_pos.y += button_size.y * i;

        // Draw the button background
        ImU32 button_color = IM_COL32(200, 200, 200, 255); // Light gray

        bool hovered = false;

        ImVec2 mousePos = ImGui::GetMousePos();
        if(mousePos.x > button_pos.x && mousePos.x < button_pos.x + button_size.x &&
           mousePos.y > button_pos.y && mousePos.y < button_pos.y + button_size.y){

            glfwSetCursor(GetWindow(), glfwCreateStandardCursor(GLFW_HAND_CURSOR));


            hovered = true;
            if(ImGui::IsMouseClicked(ImGuiMouseButton_Left) && selectedUser != i){
                scroll = true;
                selectedUser = i;
            }
        }

        ImVec2 text1_size = io.Fonts->Fonts[0]->CalcTextSizeA(16.0f, FLT_MAX, 0.0f, currentClient.GetActiveUsers()[i].pseudoName.c_str());
        ImVec2 text1_pos = ImVec2(30 + button_pos.x, (button_size.y - text1_size.y) * 0.5f + button_pos.y - 5);

        //std::string psuedoName;
            // try {
            //     int number = 2; //std::stoi(currentClient.GetActiveUsers()[i].publicKey);
            //     psuedoName = GetPsuedoName();
            // } catch (const std::invalid_argument& e){
            //     psuedoName = currentClient.GetActiveUsers()[i].publicKey;
            // } catch (const std::out_of_range& e){
            //     psuedoName = currentClient.GetActiveUsers()[i].publicKey;
            // }

        if(i == selectedUser){
            int currentColourIndex = currentClient.GetColourIndex(currentClient.GetActiveUsers()[selectedUser].pseudoName);
            ImVec4 currentColour = colourVectorU32[currentColourIndex];
            
            draw_list->AddRectFilled(button_pos, ImVec2(button_pos.x + button_size.x, button_pos.y + button_size.y), IM_COL32(currentColour.x, currentColour.y, currentColour.z, 30));
            draw_list->AddRectFilled(ImVec2(button_pos.x + button_size.x - 4, button_pos.y), ImVec2(button_pos.x + button_size.x, button_pos.y + button_size.y), IM_COL32(currentColour.x, currentColour.y, currentColour.z, 255), 0.0f, 0); // Border
            

            draw_list->AddText(fontList[FONT_PRIMARY].imguiFontRef, (float)fontList[FONT_PRIMARY].characterSize, text1_pos, IM_COL32(currentColour.x, currentColour.y, currentColour.z, 255), currentClient.GetActiveUsers()[selectedUser].pseudoName.c_str());

        }
        else if(hovered){
            draw_list->AddRectFilled(button_pos, ImVec2(button_pos.x + button_size.x, button_pos.y + button_size.y), GLOBAL_BACKGROUND_EXTRA_LIGHT_COLOUR_U32);
            draw_list->AddText(fontList[FONT_PRIMARY].imguiFontRef, (float)fontList[FONT_PRIMARY].characterSize, text1_pos, IM_COL32(255,255,255,255), currentClient.GetActiveUsers()[i].pseudoName.c_str());
        }
        else{
            draw_list->AddRectFilled(button_pos, ImVec2(button_pos.x + button_size.x, button_pos.y + button_size.y), GLOBAL_BACKGROUND_COLOUR_U32);
            draw_list->AddText(fontList[FONT_PRIMARY].imguiFontRef, (float)fontList[FONT_PRIMARY].characterSize, text1_pos, IM_COL32(255,255,255,255), currentClient.GetActiveUsers()[i].pseudoName.c_str());
        }

        int newSelectedUser = selectedUser;

    }

    // end scroll area if required
    if(childSize.y > windowSize.y){
        ImGui::EndChild();
    }

    ImGui::PopStyleVar(4); 
    ImGui::PopStyleColor(2);

}

void ChatApplication::DrawConnectToServerModal(){

    // no modal if already connected
    if(connectedState == CS_CONNECTED){
        return;
    }

    static char serverAddressBuffer[256] = "";
    static char serverPortBuffer[256] = "";
    static bool addressInput = false;
    static bool portInput = false;
    static bool focusOnPort = false;
    static bool showWarning = false;



    ImVec2 modalSize = ImVec2(400,200);

    // Set size of the modal
    ImGui::SetNextWindowSize(modalSize, ImGuiCond_Always);

    // Center the modal on the screen
    ImVec2 windowPos = ImVec2((ImGui::GetIO().DisplaySize.x - modalSize.x) * 0.5f,
                              (ImGui::GetIO().DisplaySize.y - modalSize.y) * 0.5f);

    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);

    if(ImGui::BeginPopupModal("Select Server", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize)){
        

        // we are yet to connect
        if(connectedState != CS_IN_PROGRESS){

            // connection modal content
            
            ImGui::Text("Address");
            if(ImGui::InputText("##Address", serverAddressBuffer, IM_ARRAYSIZE(serverAddressBuffer), ImGuiInputTextFlags_EnterReturnsTrue)){

                addressInput = strlen(serverAddressBuffer) > 0;

                if(addressInput){
                    focusOnPort = true;
                    showWarning = false;
                } else {
                    ImGui::SetKeyboardFocusHere(-1);
                }
            }

            if(focusOnPort){
                ImGui::SetKeyboardFocusHere();
                focusOnPort = false;
            }

            ImGui::Text("Port");
            if(ImGui::InputText("##Port", serverPortBuffer, IM_ARRAYSIZE(serverPortBuffer), ImGuiInputTextFlags_EnterReturnsTrue)){
    
                portInput = strlen(serverPortBuffer) > 0;
                addressInput = strlen(serverAddressBuffer) > 0;

                if(portInput && addressInput){
                    std::string address(serverAddressBuffer);
                    std::string port(serverPortBuffer);
                    std::string finalAddress = address + ":" + port;

                    serverAddressToJoin = finalAddress;

                    ImGui::CloseCurrentPopup();
                    socketThread = std::thread(&ClientSocket::Start, &socket, address, port); 

                } else {
                    showWarning = true;
                    ImGui::SetKeyboardFocusHere(-1);
                }
            }

            ImGui::Spacing();
            
            addressInput = strlen(serverAddressBuffer) > 0;
            portInput = strlen(serverPortBuffer) > 0;

            if(ImGui::Button("Connect") && addressInput && portInput){
                std::string address(serverAddressBuffer);
                std::string port(serverPortBuffer);
                std::string finalAddress = address + ":" + port;

                ImGui::CloseCurrentPopup();
                socketThread = std::thread(&ClientSocket::Start, &socket, address, port); 
            }

            if(showWarning){
                ImGui::Spacing();
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Please enter a valid Address and Port.");
            }
        }
        else{
            std::string connectingText = "Connecting to " + serverAddressToJoin + "...";
            ImGui::Text(connectingText.c_str());
        }


        ImGui::EndPopup();
    }
}

void ChatApplication::Update(){

    if(connectedState == CS_DISCONNECTED){
        ImGui::OpenPopup("Select Server");
    }

    static char inputBuffer[256] = "";  // Buffer to hold the input text
    static bool scroll = true;
    
    //glfwSetCursor(GetWindow(), glfwCreateStandardCursor(GLFW_ARROW_CURSOR));

    if(currentClient.GetActiveUsers().size() > 0 && (currentClient.GetActiveUsers()[0].publicKey != "")){
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
        ImGui::Begin("Users", nullptr); // Create a new ImGui window
        
        DrawCustomUserButtons(scroll);

        ImGui::End(); // End the ImGui window
        ImGui::PopStyleVar(1);
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(GLOBAL_PADDING, GLOBAL_PADDING));
    ImGui::Begin("Messages", nullptr, ImGuiWindowFlags_NoScrollbar); 
    ImGui::PopStyleVar(1);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(GLOBAL_PADDING, GLOBAL_PADDING));

    ImVec2 contentRegion = ImGui::GetContentRegionAvail();
    float inputBoxHeight = 30.0f; // Adjust this value according to your input box height
    ImVec2 scrollAreaSize = ImVec2(contentRegion.x, contentRegion.y - 60); // Reserve space for the input box

    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 20);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(GLOBAL_PADDING, GLOBAL_PADDING));
    ImGui::PushStyleColor(ImGuiCol_Border, GLOBAL_BACKGROUND_EXTRA_LIGHT_COLOUR);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10);
    ImGui::BeginChild("MessagesScrollArea", scrollAreaSize, true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
    

    auto messages = currentClient.GetAllMessages();

    for(int i = 0; i < messages[currentClient.GetActiveUsers()[selectedUser].publicKey].size(); i++){
        
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,5));
        
        if(currentClient.GetChatMessage(currentClient.GetActiveUsers()[selectedUser].publicKey, i).sentBy == "me (" + currentPseudoName +")"){
            PushFont(FONT_BOLD, colourVector[5]);
        } else {
            PushFont(FONT_BOLD, colourVector[currentClient.GetColourIndex(currentClient.GetChatMessage(currentClient.GetActiveUsers()[selectedUser].publicKey, i).sentBy)]);
        }

        ImGui::Text(currentClient.GetChatMessage(currentClient.GetActiveUsers()[selectedUser].publicKey, i).sentBy.c_str());
        ImGui::SameLine();
        ImGui::Text("   ");
        ImGui::SameLine();
        PopFont();

        // draw date
        PushFont(FONT_SECONDARY);
        ImGui::Text(currentClient.GetChatMessage(currentClient.GetActiveUsers()[selectedUser].publicKey, i).date.c_str());
        PopFont();

        ImGui::PopStyleVar(1);
        
        PushFont(FONT_PRIMARY);

        // draw file
        if(currentClient.GetChatMessage(currentClient.GetActiveUsers()[selectedUser].publicKey, i).isFile){

            // open file on click
            if(ImGui::Button(currentClient.GetChatMessage(currentClient.GetActiveUsers()[selectedUser].publicKey, i).filename.c_str())){
                ClientSocket::OpenLinkInBrowser(currentClient.GetChatMessage(currentClient.GetActiveUsers()[selectedUser].publicKey, i).fileURL.c_str());
            }

        }
        else{ // draw message
            ImGui::Text(currentClient.GetChatMessage(currentClient.GetActiveUsers()[selectedUser].publicKey, i).message.c_str());
        }        
        PopFont();

    }

    if(scroll){
        ImGui::SetScrollHereY(1.0f);
        scroll = false;
    }

    ImGui::EndChild(); // End the scrollable area
    ImGui::PopStyleVar(3);
    ImGui::PopStyleVar(1);
    ImGui::PopStyleColor(1);

    // Add a separator and a text box at the bottom of the window

    if(currentClient.GetActiveUsers().size() > 0 && (currentClient.GetActiveUsers()[0].publicKey != " ")){
        ImGui::SetNextItemWidth(contentRegion.x);
    }

    if(ImGui::Button("Upload File")){
        socket.SelectFile();
    }

    if (ImGui::InputText("##MessageInput", inputBuffer, IM_ARRAYSIZE(inputBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {

        // Process the input here, for example, send the message
        std::string newMessage(inputBuffer);
        
        if (!newMessage.empty()) {
            // Add the new message to the selected user's message list (pseudo-code)

            // Sending chat message to server
            socket.SendChatMessage(newMessage, currentClient.GetActiveUsers()[selectedUser].publicKey);

            currentClient.PushMessage({newMessage, "me (" + currentPseudoName +")", GetCurrentDateTime(false)}, currentClient.GetActiveUsers()[selectedUser].publicKey);

            selectedUser = currentClient.UpdateDate(currentClient.GetActiveUsers()[selectedUser].publicKey, GetCurrentDateTime(true), currentClient.GetActiveUsers()[selectedUser].publicKey);

            // Clear the input buffer
            inputBuffer[0] = '\0';
            ImGui::SetKeyboardFocusHere(-1);
            scroll = true;
        }
    } 
    ImGui::End();

    DrawConnectToServerModal();

}

void ChatApplication::End() {
    if(connectedState == CS_CONNECTED){
        socket.End();
        socketThread.join();
    }
}


