#pragma once
#include "Program.h"
#include <vector>

class ChatApplication : public Program{

    public:
        
        void Start() override;
        void Update() override;

    private:

        
        std::vector<std::string> users;
        int selectedUser;

};