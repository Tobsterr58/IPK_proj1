/*
    IPK project 1 (IPK Calculator Protocol Client)
    Tobiáš Štec (xstect00)
    2BIT - 2023
*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <getopt.h>
#include <math.h>

#define BUFSIZE 1024

using namespace std;

int client_socket; //can be also not global but its easier to use it in signal handler

//signal handler for TCP (CTRL+C)
void handle_sigint_TCP(int sig) {
    send(client_socket, "BYE\n", strlen("BYE\n"), 0);
    char buf[BUFSIZE];
    recv(client_socket, buf, BUFSIZE, 0);
    printf("%s", buf);
    close(sig);
    exit(0);
}

//tcp function with signal handler for CTRL+D and CTRL+C
void tcpitis(int client_socket, char *buf, struct sockaddr_in server_address, string endit, int bytestx, int bytesrx)
{
    int controlPressed = 0;
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = handle_sigint_TCP;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    if (connect(client_socket, (const struct sockaddr *) &server_address, sizeof(server_address)) != 0)
        {
            perror("ERROR: connect");
            exit(EXIT_FAILURE);        
        }

    while (endit!=buf)
    {   
        bzero(buf, BUFSIZE); //clears buffer
        if (controlPressed == 0 && feof(stdin)) {
            buf= "BYE\n";
        }
        else {
        fgets(buf, BUFSIZE, stdin); //gets input from stdin
        }    
        int buf_len = strlen(buf);
        buf[buf_len]=(char)10; //adds new line to the end of input

        bytestx = send(client_socket, buf, strlen(buf), 0); //sends input to server
        if (bytestx < 0) 
        perror("ERROR in sendto");
        bzero(buf, BUFSIZE);
        
        bytesrx = recv(client_socket, buf, BUFSIZE, 0); //gets response from server
        if (bytesrx < 0) 
        perror("ERROR in recvfrom");
        
        printf("%s", buf);
        if (endit==buf)
        {
            close(client_socket);
        }
    }
}

//signal handler for UDP (CTRL+C)
void handle_sigint_UDP(int sig) {
    printf("\n");
    close(sig);
    exit(0);
}

//udp function with signal handler for CTRL+D and CTRL+C
void udpitis(int client_socket, char *buf, struct sockaddr_in server_address, int bytestx, int bytesrx) {
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = handle_sigint_UDP;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    socklen_t server_address_length = sizeof(server_address);
    bool done = false;

    while (!done) {
        bzero(buf, BUFSIZE);
        fgets(buf + 2, BUFSIZE - 2, stdin);

        if (feof(stdin)) { // CTRL+D
            printf("\n");
            close(client_socket);
            done = true;
            continue;
        }

        int msg_len = strlen(buf + 2); // +2 because of the first two bytes
        buf[0] = '\0'; // first byte is always 0
        buf[1] = (char) msg_len; // second byte is the length of the message

        bytestx = sendto(client_socket, buf, msg_len + 2, 0, (struct sockaddr *) &server_address, server_address_length); //sends input to server
        if (bytestx < 0) {
            perror("ERROR in sendto");
            continue;
        }
        bzero(buf, BUFSIZE);

        bytesrx = recvfrom(client_socket, buf, BUFSIZE, 0, (struct sockaddr *) &server_address, &server_address_length); //gets response from server
        if (bytesrx < 0) {
            perror("ERROR in recvfrom");
            continue;
        }

        // Handle the status code and response from the server
        int status_code = (int) buf[1];
        int response_length = (int) buf[2];

        if (status_code == 0) {
            printf("OK:%.*s\n", response_length, buf + 3);
        } else {
            printf("ERR:%.*s\n", response_length, buf + 3);
        }
    }
}


int main(int argc, char *argv[])
{

int arg=0;
string host;
string port;
string mode;
int port_number, bytestx, bytesrx;
const char *server_hostname;
struct hostent *server;
struct sockaddr_in server_address;

string endit = "BYE\n";

char buf[BUFSIZE];

//getopt_long for parsing arguments
const struct option longopts[] = 
  {
    {"host",      required_argument,  0, 'h'},
    {"port",      required_argument,  0, 'p'},
    {"mode",      required_argument,  0, 'm'},
    {0,0,0,0}
  };
    if (argc != 7) //checks if there are 7 arguments
    {
        fprintf(stderr, "Usage: [-h host] [-p port] [-m mode]\n");
        return EXIT_FAILURE;
    }
    
    while ((arg = getopt_long(argc, argv, "h:p:m:", longopts, NULL))!=-1) //parses arguments
    {
        switch (arg)
        {
            case 'h':
                host = optarg;
                break;
            case 'p':
                port = optarg;
                break;
            case 'm':
                mode = optarg;
                break;
            default:
                fprintf(stderr, "Usage: [-h host] [-p port] [-m mode]\n");
                return EXIT_FAILURE;
                break;  
        }
    }


    if (host.empty() || port.empty() || mode.empty()) //checks if arguments are empty
    {
        fprintf(stderr, "Usage: [-h host] [-p port] [-m mode]\n");
        return EXIT_FAILURE;
    }

    port_number = atoi(port.c_str()); //converts port to int
    server_hostname = host.c_str(); //converts host to const char*

    if ((server = gethostbyname(server_hostname)) == NULL) { //checks if host is valid
        fprintf(stderr,"ERROR: no such host as %s\n", server_hostname);
        exit(EXIT_FAILURE);
    }


    bzero((char *) &server_address, sizeof(server_address)); //clears server_address
    server_address.sin_family = AF_INET; //sets server_address to AF_INET (IPv4) ... AF_INET6 for IPv6 but we don't need that because of the assignment
    bcopy((char *)server->h_addr, (char *)&server_address.sin_addr.s_addr, server->h_length); //copies server address to server_address
    server_address.sin_port = htons(port_number); //sets port number to server_address

    //checks if mode is tcp or udp
    if (mode == "tcp")
    {
        if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
        {
            perror("ERROR: socket");
            exit(EXIT_FAILURE);
        } 
        tcpitis(client_socket, buf, server_address, endit, bytestx, bytesrx);     
    }
    else if (mode == "udp")
    {
        if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) <= 0)
        {
            perror("ERROR: socket");
            exit(EXIT_FAILURE);
        }
        udpitis(client_socket, buf, server_address, bytestx, bytesrx);
    }
    else
    {
        fprintf(stderr, "ERROR: wrong mode\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}
