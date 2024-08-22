#include "ChatApplication.h"

void ChatApplication::Start(){

    Configure_FontList();

    users = {
            "lexi",
            "ash",
            "max",
            "john star",
            "tony butcher",
            "catlover123",
            "SAZY11",
    };
    selectedUser = 0;


    messages = {
        ChatMessage({"Hey guys how is everyone"}),
        ChatMessage({"No way! i'm the coolest cat ever wow wow wow"}),
        ChatMessage({"You sure are a cool cat, aren't youu the coolest"}),
        ChatMessage({"No way! i'm the coolest cat ever wow wow wow"}),
        ChatMessage({"You sure are a cool cat, aren't youu the coolest! You sure are a cool cat, aren't youu the coolest! You sure are a cool cat, aren't youu the coolest! You sure are a cool cat, aren't youu the coolest!"}),ChatMessage({"Hey guys how is everyone"}),
        ChatMessage({"No way! i'm the coolest cat ever wow wow wow"}),
        ChatMessage({"You sure are a cool cat, aren't youu the coolest"}),
        ChatMessage({"No way! i'm the coolest cat ever wow wow wow"}),
        ChatMessage({"You sure are a cool cat, aren't youu the coolest! You sure are a cool cat, aren't youu the coolest! You sure are a cool cat, aren't youu the coolest! You sure are a cool cat, aren't youu the coolest!"}),

    };

}

void ChatApplication::Configure_FontList(){

    // FONT_PRIMARY
    FontData primaryFont;
    primaryFont.path = "Client/assets/fonts/gg sans Regular.ttf";
    primaryFont.colour = ImVec4(1.0,1.0,1.0,1.0);
    primaryFont.characterSize = 20;

    // FONT_SECONDARY
    FontData secondaryFont;
    secondaryFont.path = "Client/assets/fonts/gg sans Regular.ttf";
    secondaryFont.colour = ImVec4(1.0,1.0,1.0,0.7);
    secondaryFont.characterSize = 14;


    fontList.push_back(primaryFont);
    fontList.push_back(secondaryFont);


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

void ChatApplication::PushFont(Font font_index){
    ImGui::PushFont(fontList[font_index].imguiFontRef); // font
    ImGui::PushStyleColor(ImGuiCol_Text, fontList[font_index].colour);// font colour
}
void ChatApplication::PopFont(){
    ImGui::PopFont();
    ImGui::PopStyleColor(1);
}


void ChatApplication::Update(){

    
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

        for(int i = 0; i < users.size(); i++){

            if(i == selectedUser){
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.45f, 0.45f, 0.55f, 1.00f)); 
            }

            int newSelectedUser = selectedUser;
            if (ImGui::Button(users[i].c_str(), ImVec2(buttonWidth, 0))){ // Add a button
                newSelectedUser = i;
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
    ImGui::Begin("Messages", nullptr); 
    ImGui::PopStyleVar(1);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,25));
    
    for(int i = 0; i < messages.size(); i++){
        
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,5));
        
        // draw date
        PushFont(FONT_SECONDARY);
        ImGui::Text(messages[i].date.c_str());
        PopFont();

        ImGui::PopStyleVar(1);

        
        PushFont(FONT_PRIMARY);
        ImGui::Text(messages[i].message.c_str());
        PopFont();
        
    }

    ImGui::PopStyleVar(1);

    ImGui::End();

}

