# Chat Application

Protocol Specification https://github.com/xvk-64/2024-secure-programming-protocol

Project contains both a client program and server program

Compile client with `make client` run with `client.exe`

Compile client with `make server` run with `server.exe`

# Windows Installiation 

Install MinGW32 
Add to system path 

To install make
Install scoop
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
Invoke-RestMethod -Uri https://get.scoop.sh | Invoke-Expression

scoop bucket add main        // Might already exist if so just continue
scoop install main/make
