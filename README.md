# Chat Application

*A non-persistant chat application utalizing a custom student created protocol.*

Protocol Specification <a href="https://github.com/xvk-64/2024-secure-programming-protocol" target="_blank">here</a>

![Visual Example](Visual.png)

below you will find instructions on how to:
* Install dependencies for Windows (what we did)
* Compile the server and client
* Run the server

# Windows Installiation 

### Install Mingw32 (MinGW-W64-builds-5.0.0) 

We could were unable to find this exact build so we've uploaded a zip file <a href="https://drive.google.com/file/d/1OWfUZDzO-zTUAAP0gxaqcdBLwayYyLix/view?usp=sharing" target="_blank">here</a>
(if you do it this way follow the below instructions to add it to your system path) 

1. 	Search "edit the system environment variables" and click the control panel result 
2.	Click "Environment Variables..." at the bottom right of window
3.	Under "User variables" edit the "Path" variable
4.	Add the path to Mingw32/bin to the variable (e.g. C:\Program Files\mingw32\bin)
5.	Press OK
6.	Under "System variables" edit the "Path" variable
7.	Repeat steps 4 - 5

### Install Scoop (https://scoop.sh/)

1. Open a PowerShell terminal
2. Run "Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser"
3. Run "Invoke-RestMethod -Uri https://get.scoop.sh | Invoke-Expression"

###	Install Make (https://scoop.sh/#/apps?q=make)

1. Open a PowerShell terminal
2. Run "scoop bucket add main" (may already exist, if so continue to step 3)
3. Run "scoop install main/make"

# Compiling

Use the following comands to compile the client and server
`make client`
`make server`

Use the following commands to run the client and server
`client.exe`
`server.exe`  

>**NOTE: Please read how to run the server before actually trying to run it**

# Running The Server

Before running the server.exe please read the following.

There are three important .txt files you need to edit before running a server 

### Server Properties.txt
This text file contains one line of the server's public ip (NOT including port).
You can use 127.0.0.1 if you want to test using your local machine

### Server List.txt
This text file contains a line for each server address (e.g. 127.0.0.1:4563) you want to automatically connect to upon running the server.
The server will attempt to connect every 10 seconds if a connection is not established immediately.

### Server Whitelist.txt
This text file contains the public keys of the only servers that are allowed to connect. 
The public keys are seperated with "-----START PUBLIC KEY-----" and "-----END PUBLIC KEY-----".

When running server.exe you will be prompted to enter a port (this is to allow for easy testing on a local machine).
Upon entering this port a (ip)..(port)Keys.txt file will be created with the server's keys, which can then be used for the "Server Whitelist.txt"

If you want to connect to a server that was NOT listed in "Server List.txt" type "connect (address)" replacing (address)
with the destination address (e.g. 127.0.0.1:4563)  


