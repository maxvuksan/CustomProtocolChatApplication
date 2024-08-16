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
all:
	$(CXX) $(CXXFLAGS) $(SRCS) -o main.exe $(LDFLAGS)

# Copy DLL to output directory (adjust if needed)
copy_dll:
	cp ./glfw3.dll main.exe

# Build and copy DLL
build: all copy_dll

# Clean up build files
clean:
	rm -f main.exe

# Phony targets
.PHONY: all clean copy_dll build
