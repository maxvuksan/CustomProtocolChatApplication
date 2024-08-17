#include "ChatApplication.h"

void ChatApplication::Start(){

    std::cout << "Chat App Started!";

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

}

void ChatApplication::Update(){

    // Max's example ImGui window! ----------------------------------------------------------------------

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
    ImGui::Begin("Example Window"); // Create a new ImGui window
    
        ImGui::Text("Hello, world!");   // Display some text
        
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





    ImGui::Begin("Example Window 3"); // Create a new ImGui window
    
        ImGui::Text("Hello, world!");   // Display some text
        
        if (ImGui::Button("Click me"))  // Add a button
        {
            std::cout << "Button clicked!" << std::endl;
        }

    ImGui::End(); // End the ImGui window

    ImGui::Begin("Example 2"); // Create a new ImGui window
    
        ImGui::Text("Hello, world!");   // Display some text
        
        if (ImGui::Button("Click me"))  // Add a button
        {
            std::cout << "Button clicked!" << std::endl;
        }

    ImGui::End(); // End the ImGui window

}

