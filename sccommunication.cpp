// Written by Avinoam Nukrai and Idan Shponder- EX5 OS course Hebrew U 2022 spring

// Imports
#include <iostream>
#include <string>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <cstring>
using namespace std;


// Constants
#define SERVER_COMMAND "server"
#define ACCEPT_ERROR "accept function were failed"
#define READ_ERROR "read function was failed"
#define SYSTEM_ERROR(info) "system error: " << (info) << endl;
#define CLIENT_COMMAND "client"
#define WRONG_INPUT_NUMBER "wrong inputs number"
#define WRONG_WORKPLACE "invalid socket workPlace has given"
#define SYS_ERROR "problem in system error"
#define BIND_ERROR "bind function was failed!"
#define LISTEN_ERROR "listen function was failed!"
#define CLOSE_ERROR "close function was failed!"
#define CONNECT_ERROR "connect function was failed!"
#define WRITE_ERROR "write function was failed!"
#define GETHOSTNAME_ERROR "gethostname func was failed!"
#define GETHOSTNAMEBYNAME_ERROR "gethostnamebyname func was failed!"
#define SOCKET_ERROR "socket function was failed!"
#define MAX_CLIENT_NUM 5
#define MAX_HOST_NAME 256



/**
 * This function is the error handle function of the program
 * @param msg - msg to print to the the cerr
 */
void error_handle(const string& msg){
    cerr << SYSTEM_ERROR(msg)
    exit(EXIT_FAILURE);
}

/**
 * This function is responsible for reading the data from some socket into some buffer
 * @param socket - to read from
 * @param buff - to read into
 * @param buffLen - the len of the buffer
 * @return the number of bytes that we read
 */
int read_data(int socket, char* buff, int buffLen){
    int b_count = 0; // counter for how many bytes we already read
    int br; // counter for the num of bytes we read in one phase
    while (b_count < buffLen){
        br = (int)read(socket, buff, buffLen - b_count);
        if (br > 0){
            b_count += br;
            buff += br;
        } else if (br == -1){
            close(socket);
            return -1;
        }
        else{ break; }
    }
    return b_count;
}


/**
 * This function responsible for create a socket, each for the server and for some client
 * @param socket_fd - number in which we will store the socket itself
 * @param port_num - the port number of our host
 * @param sa - the socket address, where we will store all the data about the socket
 * @return if success or failed
 */
int create_socket(int &socket_fd, unsigned short port_num, struct sockaddr_in& sa) {
    struct hostent *host_info;
    char hostname[MAX_HOST_NAME];
    // host_info init
    if (gethostname(hostname, MAX_HOST_NAME) < 0) error_handle(GETHOSTNAME_ERROR);
    if ((host_info = gethostbyname(hostname)) == nullptr) error_handle(GETHOSTNAMEBYNAME_ERROR);
    // sockaddr_in init
    memset(&sa, 0, sizeof(sa));
    memcpy((char *) &sa.sin_addr, host_info->h_addr, host_info->h_length);
    sa.sin_family = host_info->h_addrtype;
    sa.sin_port = htons((u_short) port_num);
    // create the socket
    if ((socket_fd = socket(host_info->h_addrtype, SOCK_STREAM, 0)) < 0) error_handle(SOCKET_ERROR);
    return EXIT_SUCCESS;
}


/**
 * This function creates the socket of the server and return if succeeded or not
 * @param port_num - the port number to work with
 * @return indicator if we succeeded to create the socket
 */
int build_server_socket(unsigned short port_num){
    int server_socket;
    int client_server;
    struct sockaddr_in socket_addr{};
    char client_command[MAX_HOST_NAME];

    // creates a socket for the server
    create_socket(server_socket, port_num, socket_addr);

    // bind the socket
    if ((bind(server_socket, reinterpret_cast<const struct sockaddr *>(&socket_addr), sizeof(socket_addr)) < 0)){
        close(server_socket);
        error_handle(BIND_ERROR);
    }

    // define the max number of clients that can connect same time to the server
    if (listen(server_socket, MAX_CLIENT_NUM) < 0){
        close(server_socket);
        error_handle(LISTEN_ERROR);
    }

    // accept clients and actually run their commands
    while ((client_server = accept(server_socket, nullptr, nullptr)) != -1){
        if (client_server == -1){
            close(server_socket);
            error_handle(ACCEPT_ERROR);
        }
        memset(client_command, 0, sizeof(client_command));
        // read the command
        if(read_data(client_server, client_command, MAX_HOST_NAME) == -1){
            close(server_socket);
            close(client_server);
            error_handle(READ_ERROR);
        }
        // run the command
        if (system(client_command) == -1){
            close(server_socket);
            close(client_server);
            error_handle(SYS_ERROR);
        }
    }

    // close the right socket before leaving
    if (close(server_socket) < 0 && close(client_server) < 0){
        error_handle(CLOSE_ERROR);
        return -1;
    }
    return EXIT_SUCCESS;
}


/**
 * This function responsible for create the socket of the clients and write into it the command the client want to run
 * @param port_num - the port number of our program
 * @param client_command - the command of the client - given in the cli arguments
 * @return indicator if we succeeded or not
 */
int build_client_socket(unsigned short port_num, const char* client_command){
    int client_socket;
    struct sockaddr_in sa{};

    // create a socket for the client
    create_socket(client_socket, port_num, sa);

    // connect the client to the socket of the server
    if (connect(client_socket, (struct sockaddr*)&sa, sizeof(sa)) == -1){
        close(client_socket);
        error_handle(CONNECT_ERROR);
    }

    // write the client command to his socket
    if (write(client_socket, client_command, strlen(client_command) + 1) < 0) {
        close(client_socket);
        error_handle(WRITE_ERROR);
    }

    // closing the client socket
    if (close(client_socket) < 0) return -1;
    return EXIT_SUCCESS;
}


/**
 * This function is the main func of the program - responsible for pars the arguments and fulfill the user wish - to
 * run a server or client with some well define command to run
 * @param argc - num of args
 * @param argv - the args of the cli
 * @return indicator if succeeded or not
 */
int main(int argc, char* argv[]){
    // input validation
    if (argc < 3) error_handle(WRONG_INPUT_NUMBER);
    // pars args
    char curHostname[MAX_HOST_NAME + 1];
    gethostname(curHostname, MAX_HOST_NAME);
    unsigned short port_num = strtol(argv[2], nullptr, 10);
    if (strcmp(argv[1], CLIENT_COMMAND) == 0){
        build_client_socket(port_num, argv[3]);
    }
    else if (strcmp(argv[1], SERVER_COMMAND) == 0){
        build_server_socket(port_num);
    }
    else {
        cerr << SYSTEM_ERROR(WRONG_WORKPLACE)
        return EXIT_FAILURE;
    }
}