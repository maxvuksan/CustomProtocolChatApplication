#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

/*
    Abstracting away the GLFW setup
*/
class Program {

    public:
        Program(int windowWidth = 1000, int windowHeight = 500, std::string windowTitle = "Program");
        ~Program();

        // called right before Run()
        virtual void Start(){}
        // called every frame of Run()
        virtual void Update(){}

        void Run();


        int GetWindowWidth() { return windowWidth; }
        int GetWindowHeight() { return windowHeight; }

        
    protected:

    


    private:

        /*
            is triggered when the window resizes
        */
        static void Framebuffer_Size_Callback(GLFWwindow *window, int width, int height);

        
        GLFWmonitor* monitor;
        GLFWwindow* window;

        static int windowWidth;
        static int windowHeight;

};