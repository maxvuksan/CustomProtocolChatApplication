

SHARED_INCLUDE = -I./include
SHARED_LIBS = -L./lib -lws2_32 -lmswsock -lcryptopp
SHARED_FLAGS = -std=c++17 -DASIO_STANDALONE -D_WEBSOCKETPP_CPP11_THREAD_

# forcing make client and make server to always make
FORCE:

# Client ------------------------------------------------------------------------------------------------

# Libraries
CLIENT_LIBS = -lglfw3dll -lopengl32

CLIENT_SRCS = ./Client/src/glad.c \
       ./Client/src/*.cpp \
       ./Client/include/imgui/*.cpp \

# Compile and link in one step
client: FORCE
	g++ $(CLIENT_SRCS) $(CLIENT_LIBS) -o client.exe -I./Client/include -L./Client/lib $(SHARED_INCLUDE) $(SHARED_LIBS) $(SHARED_FLAGS)




# Server ------------------------------------------------------------------------------------------------

SERVER_SRCS = ./Server/src/*.cpp \

# Compile and link in one step
server: FORCE
	g++ $(SERVER_SRCS) -o server.exe $(SHARED_INCLUDE) $(SHARED_LIBS) $(SHARED_FLAGS)


