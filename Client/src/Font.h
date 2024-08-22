#pragma once
#include "Program.h"

/*
    Allows fonts to be specified through an enum
*/
enum Font{

    FONT_PRIMARY,
    FONT_SECONDARY,

    FONT_NUMBER_OF_FONTS, // total number of font types
};

/*
    The associated data for each font enum
*/
struct FontData{

    std::string path;
    int characterSize = 16;
    ImVec4 colour = ImVec4(1.0,1.0,1.0,1.0);

    ImFont* imguiFontRef;
};