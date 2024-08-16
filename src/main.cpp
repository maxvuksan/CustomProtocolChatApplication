#include <string>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui.h"  // Required to use ImGui functions

#include <iostream>
#include <vector>

std::string programName = "GLFW window";
int windowWidth = 1200,
    windowHeight = 800;
float backgroundR = 0.1f,
      backgroundG = 0.3f,
      backgroundB = 0.2f;

static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void teardown(GLFWwindow *window)
{
    if (window != NULL) { glfwDestroyWindow(window); }
    glfwTerminate();
}

int main(int argc, char *argv[])
{
    if (!glfwInit())
    {
        std::cerr << "[ERROR] Couldn't initialize GLFW\n";
        return -1;
    }
    else
    {
        std::cout << "[INFO] GLFW initialized\n";
    }

    // Setup GLFW window hints
    glfwWindowHint(GLFW_DOUBLEBUFFER , 1);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    
    float highDPIscaleFactor = 1.0;
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    float xscale, yscale;
    glfwGetMonitorContentScale(monitor, &xscale, &yscale);
    if (xscale > 1 || yscale > 1)
    {
        highDPIscaleFactor = xscale;
        glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    }

    GLFWwindow *window = glfwCreateWindow(
        windowWidth,
        windowHeight,
        programName.c_str(),
        NULL,
        NULL
    );
    if (!window)
    {
        std::cerr << "[ERROR] Couldn't create a GLFW window\n";
        teardown(NULL);
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable VSync

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "[ERROR] Couldn't initialize GLAD" << std::endl;
        teardown(window);
        return -1;
    }
    else
    {
        std::cout << "[INFO] GLAD initialized\n";
    }

    // Initialize ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Setup ImGui style
    ImGui::StyleColorsDark();

    // Set framebuffer size callback
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);









    int theme = 0;

    if(theme == 0){
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 5.3f;
        style.FrameRounding = 2.3f;
        style.ScrollbarRounding = 0;

        style.Colors[ImGuiCol_Text]                  = ImVec4(0.90f, 0.90f, 0.90f, 0.90f);
        style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
        style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.09f, 0.09f, 0.15f, 1.00f);
        style.Colors[ImGuiCol_PopupBg]               = ImVec4(0.05f, 0.05f, 0.10f, 0.85f);
        style.Colors[ImGuiCol_Border]                = ImVec4(0.70f, 0.70f, 0.70f, 0.65f);
        style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_FrameBg]               = ImVec4(0.00f, 0.00f, 0.01f, 1.00f);
        style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.90f, 0.80f, 0.80f, 0.40f);
        style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.90f, 0.65f, 0.65f, 0.45f);
        style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.00f, 0.00f, 0.00f, 0.83f);
        style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
        style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.00f, 0.00f, 0.00f, 0.87f);
        style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.01f, 0.01f, 0.02f, 0.80f);
        style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
        style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.55f, 0.53f, 0.55f, 0.51f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.56f, 0.56f, 0.56f, 0.91f);
        style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.90f, 0.90f, 0.90f, 0.83f);
        style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.70f, 0.70f, 0.70f, 0.62f);
        style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.30f, 0.30f, 0.30f, 0.84f);
        style.Colors[ImGuiCol_Button]                = ImVec4(0.48f, 0.72f, 0.89f, 0.49f);
        style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.50f, 0.69f, 0.99f, 0.68f);
        style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
        style.Colors[ImGuiCol_Header]                = ImVec4(0.30f, 0.69f, 1.00f, 0.53f);
        style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.44f, 0.61f, 0.86f, 1.00f);
        style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.38f, 0.62f, 0.83f, 1.00f);
        style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(1.00f, 1.00f, 1.00f, 0.85f);
        style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
        style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
        style.Colors[ImGuiCol_PlotLines]             = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
    }


    // Enable docking
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;





    std::vector<std::string> users = {
        "lexi",
        "ash",
        "max",
        "john star",
        "tony butcher",
        "catlover123",
        "SAZY11",
    };
    int selectedUser = 0;





    // Rendering loop
    while (!glfwWindowShouldClose(window))
    {
        // Start the ImGui frame
        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();




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


        // --------------------------------------------------------------------------------------------------


        // Render

        ImGui::Render();

        glClearColor(backgroundR, backgroundG, backgroundB, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render ImGui
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glScissor(0, 0, display_w, display_h);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }





    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    teardown(window);

    return 0;
}
