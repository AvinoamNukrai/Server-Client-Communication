# Server-Client-Communication
Program for Server-Client communication


In this project I built software that allows the server and clients to communicate between them. I did this by creating appropriate sockets for each of them, the server and the clients and also linked them so that the server would execute the command that the client wants to run.
I have created an executable which based on command line arguments that runs either a client or server at a port given at the command line argument.
The server will execute terminal commands that the client passes to it through the socket.
After compilation there is an executable file called i.e. sockets, that can be run as follows:

./sockets client <port> <terminal_command_to_run>
  
./sockets server <port>

The way we actully run the program is to open 2 diffrent terminals, one for each - one for the server and one for the client, then to compile the 
code and run the above commands. We will accept to see that the server is open for getting for requests from some clients and excute them if needed. 
  
The following diagram illustrates the concept of server-to-client communication: 
  
  ![Screenshot 2022-06-20 113146](https://user-images.githubusercontent.com/64755588/174560010-5ce44593-91dd-41bb-9ef8-918337181fa5.png)
