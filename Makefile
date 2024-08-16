# Compiler
CXX = g++

# Directories
INCLUDE_DIRS = -I./include
LIB_DIRS = -L./lib

# Libraries
LIBS = -lglfw3dll -lopengl32

# Source files
SRCS = ./src/glad.c \
       ./src/main.cpp \
       ./include/imgui/imgui_impl_glfw.cpp \
       ./include/imgui/imgui_impl_opengl3.cpp \
       ./include/imgui/imgui.cpp \
       ./include/imgui/imgui_demo.cpp \
       ./include/imgui/imgui_draw.cpp \
       ./include/imgui/imgui_tables.cpp \
       ./include/imgui/imgui_widgets.cpp

# Target executable

# Compilation and linking flags
CXXFLAGS = -std=c++17 $(INCLUDE_DIRS)
LDFLAGS = $(LIB_DIRS) $(LIBS)


# Compile and link in one step
compile:
	$(CXX) $(CXXFLAGS) $(SRCS) -o main.exe $(LDFLAGS)


# Clean up build files
clean:
	rm -f main.exe
