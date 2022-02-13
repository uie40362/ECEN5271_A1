ECEN5273 Assignment 1 - UDP Client/Server

IDE used: Clion 2021.3.2 on Windows 11 with Ubuntu 18.04 WSL2 as environment

Instructions:
1. Unpack tarball and cd to the folder 'udp'
2. While in 'udp' run the following command: make
3. cd to 'udp/server' and type: './server (PORT NO)' to start server
4. cd to 'udp/client' and type: './client (IP) (PORT NO)' to connect to server

Functions:
The client/server pair has the following functions
1. put [filename] - sends a file called [filename] from client directory to server; server saves a copy of the file as [filename]
2. get [filename] - requests the file called [filename] from the server; server sends the file if it exists and a copy is stored in client
3. delete [filename] - sends a request to the server to the delete the file called [filename]; server deletes file from its local storage if file exists
4. ls - list all files in the server's local storage
5. exit - exit the client program and shutdown the server

