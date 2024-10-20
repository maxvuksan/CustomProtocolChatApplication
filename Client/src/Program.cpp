#include "Program.h"
#include "Globals.h"

int Program::windowWidth;
int Program::windowHeight;

Program::Program(int _windowWidth, int _windowHeight, std::string windowTitle){

    windowWidth = _windowWidth;
    windowHeight = _windowHeight;

    // Init OpenGL (GLFW) -------------------------------------------------------------------

    if (!glfwInit())
    {
        std::cerr << "[ERROR] Couldn't initialize GLFW\n";
        return;
    }

    // Setup GLFW window hints
    glfwWindowHint(GLFW_DOUBLEBUFFER , 1);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

    float highDPIscaleFactor = 1.0;
    this->monitor = glfwGetPrimaryMonitor();
    float xscale, yscale;
    glfwGetMonitorContentScale(this->monitor, &xscale, &yscale);
    if (xscale > 1 || yscale > 1)
    {
        highDPIscaleFactor = xscale;
        glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    }

    // create our window
    this->window = glfwCreateWindow(
        windowWidth,
        windowHeight,
        windowTitle.c_str(),
        NULL,
        NULL
    );

    if (!window)
    {
        std::cerr << "[ERROR] Couldn't create a GLFW window\n";
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable VSync

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "[ERROR] Couldn't initialize GLAD" << std::endl;
        glfwDestroyWindow(window); 
    }

    // Set framebuffer size callback
    glfwSetFramebufferSizeCallback(window, Program::Framebuffer_Size_Callback);

    // Init ImGui -------------------------------------------------------------------

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");


    // Configure style

    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowMinSize        = ImVec2( 160, 20 );
    style.FramePadding         = ImVec2( 4, 2 );
    style.ItemSpacing          = ImVec2( 6, 2 );
    style.ItemInnerSpacing     = ImVec2( 6, 4 );
    style.Alpha                = 0.95f;
    style.WindowRounding       = 4.0f;
    style.FrameRounding        = 2.0f;
    style.IndentSpacing        = 6.0f;
    style.ItemInnerSpacing     = ImVec2( 2, 4 );
    style.ColumnsMinSpacing    = 50.0f;
    style.GrabMinSize          = 14.0f;
    style.GrabRounding         = 16.0f;
    style.ScrollbarSize        = 12.0f;
    style.ScrollbarRounding    = 16.0f;

    style.Colors[ImGuiCol_Text]                  = ImVec4(0.86f, 0.93f, 0.89f, 0.78f);
    style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.86f, 0.93f, 0.89f, 0.28f);
    style.Colors[ImGuiCol_WindowBg]              = GLOBAL_BACKGROUND_COLOUR;
    style.Colors[ImGuiCol_Border]                = GLOBAL_BACKGROUND_LIGHT_COLOUR;
    style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg]               = GLOBAL_BACKGROUND_LIGHT_COLOUR;
    style.Colors[ImGuiCol_FrameBgHovered]        = GLOBAL_BACKGROUND_EXTRA_LIGHT_COLOUR;
    style.Colors[ImGuiCol_FrameBgActive]         = GLOBAL_BACKGROUND_LIGHT_COLOUR;
    style.Colors[ImGuiCol_TitleBg]               = GLOBAL_BACKGROUND_LIGHT_COLOUR;
    style.Colors[ImGuiCol_TitleBgCollapsed]      = GLOBAL_BACKGROUND_LIGHT_COLOUR;
    style.Colors[ImGuiCol_TitleBgActive]         = GLOBAL_BACKGROUND_LIGHT_COLOUR;
    style.Colors[ImGuiCol_MenuBarBg]             = GLOBAL_BACKGROUND_LIGHT_COLOUR;
    style.Colors[ImGuiCol_ScrollbarBg]           = GLOBAL_BACKGROUND_COLOUR; 
    style.Colors[ImGuiCol_ScrollbarGrab]         = GLOBAL_BACKGROUND_EXTRA_LIGHT_COLOUR;
    style.Colors[ImGuiCol_ScrollbarGrabHovered]  = GLOBAL_BACKGROUND_EXTRA_LIGHT_COLOUR;
    style.Colors[ImGuiCol_ScrollbarGrabActive]   = GLOBAL_BACKGROUND_EXTRA_LIGHT_COLOUR;
    style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_SliderGrabActive]      = GLOBAL_BACKGROUND_EXTRA_LIGHT_COLOUR;
    style.Colors[ImGuiCol_Button]                = GLOBAL_BACKGROUND_LIGHT_COLOUR; //ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_ButtonHovered]         = GLOBAL_BACKGROUND_EXTRA_LIGHT_COLOUR;
    style.Colors[ImGuiCol_ButtonActive]          = GLOBAL_BACKGROUND_EXTRA_LIGHT_COLOUR;
    style.Colors[ImGuiCol_Header]                = GLOBAL_BACKGROUND_LIGHT_COLOUR;
    style.Colors[ImGuiCol_HeaderHovered]         = GLOBAL_BACKGROUND_LIGHT_COLOUR;
    style.Colors[ImGuiCol_HeaderActive]          = GLOBAL_BACKGROUND_LIGHT_COLOUR;
    style.Colors[ImGuiCol_Separator]             = GLOBAL_BACKGROUND_LIGHT_COLOUR;
    style.Colors[ImGuiCol_SeparatorHovered]      = GLOBAL_BACKGROUND_EXTRA_LIGHT_COLOUR;
    style.Colors[ImGuiCol_SeparatorActive]       = GLOBAL_BACKGROUND_EXTRA_LIGHT_COLOUR;
    style.Colors[ImGuiCol_ResizeGrip]            = GLOBAL_BACKGROUND_LIGHT_COLOUR;
    style.Colors[ImGuiCol_ResizeGripHovered]     = GLOBAL_BACKGROUND_EXTRA_LIGHT_COLOUR;
    style.Colors[ImGuiCol_ResizeGripActive]      = GLOBAL_BACKGROUND_EXTRA_LIGHT_COLOUR;
    style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
    style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
    style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg]        = GLOBAL_BACKGROUND_LIGHT_COLOUR;
    style.Colors[ImGuiCol_PopupBg]               = GLOBAL_BACKGROUND_COLOUR;

    style.Colors[ImGuiCol_Tab]                  = GLOBAL_BACKGROUND_LIGHT_COLOUR;
    style.Colors[ImGuiCol_TabActive]            = GLOBAL_BACKGROUND_EXTRA_LIGHT_COLOUR;
    style.Colors[ImGuiCol_TabHovered]           = GLOBAL_BACKGROUND_EXTRA_LIGHT_COLOUR;
    style.Colors[ImGuiCol_TabSelectedOverline]  = GLOBAL_BACKGROUND_EXTRA_LIGHT_COLOUR;
    
    style.Colors[ImGuiCol_TabDimmedSelectedOverline]  = GLOBAL_BACKGROUND_EXTRA_LIGHT_COLOUR;
    style.Colors[ImGuiCol_TabDimmed]                  = GLOBAL_BACKGROUND_LIGHT_COLOUR;
    style.Colors[ImGuiCol_TabDimmedSelected]          = GLOBAL_BACKGROUND_EXTRA_LIGHT_COLOUR;
    style.Colors[ImGuiCol_TabSelected]          = GLOBAL_BACKGROUND_EXTRA_LIGHT_COLOUR;
    style.Colors[ImGuiCol_DockingPreview]          = GLOBAL_BACKGROUND_EXTRA_LIGHT_COLOUR;


    style.Colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.1, 0.1, 0.12, 0.75f);


    // Enable docking
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;




}


void Program::Run(){

    // CALL START
    this->Start();

    // Rendering loop
    while (!glfwWindowShouldClose(window))
    {
        // Start the ImGui frame
        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();

        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport();

        this->Update();

        // Render
        ImGui::Render();

        glClearColor(GLOBAL_BACKGROUND_COLOUR.x * 0.5, GLOBAL_BACKGROUND_COLOUR.y * 0.5, GLOBAL_BACKGROUND_COLOUR.z * 0.5, GLOBAL_BACKGROUND_COLOUR.w);
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


    End();
}




void Program::Framebuffer_Size_Callback(GLFWwindow *window, int width, int height){
    glViewport(0, 0, width, height);
    windowWidth = width;
    windowHeight = height;
}


Program::~Program(){

    

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (window != NULL) 
    { 
        glfwDestroyWindow(window); 
    }
    glfwTerminate();


}