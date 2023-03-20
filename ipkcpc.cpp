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

void handle_sigint_TCP(int sig) {
    send(client_socket, "BYE\n", strlen("BYE\n"), 0);
    char buf[BUFSIZE];
    int bytesrx = recv(client_socket, buf, BUFSIZE, 0);
    printf("Echo from server: %s", buf);
    // Close the socket
    close(client_socket);
    exit(0);
}

void tcpitis(int client_socket, char *buf, struct sockaddr_in server_address, string endit, int bytestx, int bytesrx)
{
    int controlPressed = 0;
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = handle_sigint_TCP;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    socklen_t server_address_length = sizeof(server_address);

    if (connect(client_socket, (const struct sockaddr *) &server_address, sizeof(server_address)) != 0)
        {
            perror("ERROR: connect");
            exit(EXIT_FAILURE);        
        }

    while (endit!=buf)
    {   
        bzero(buf, BUFSIZE);
        printf("Please enter msg: ");
        if (controlPressed == 0 && feof(stdin)) {
            buf= "BYE\n";
        }
        else {
        fgets(buf, BUFSIZE, stdin); 
        }    
        int buf_len = strlen(buf);
        buf[buf_len]=(char)10;

        /* odeslani zpravy na server */
        bytestx = send(client_socket, buf, strlen(buf), 0);
        if (bytestx < 0) 
        perror("ERROR in sendto");
        bzero(buf, BUFSIZE);
        
        /* prijeti odpovedi a jeji vypsani */
        bytesrx = recv(client_socket, buf, BUFSIZE, 0);
        if (bytesrx < 0) 
        perror("ERROR in recvfrom");
        
        printf("Echo from server: %s", buf);
        if (endit==buf)
        {
            close(client_socket);
        }
    }
}

void handle_sigint_UDP(int sig) {
    printf("\n");
    close(sig);
    exit(0);
}

void udpitis(int client_socket, char *buf, struct sockaddr_in server_address, string endit, int bytestx, int bytesrx) {
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = handle_sigint_UDP;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    socklen_t server_address_length = sizeof(server_address);
    bool done = false;

    while (!done) {
        bzero(buf, BUFSIZE);
        printf("Please enter msg: ");
        fgets(buf + 2, BUFSIZE - 2, stdin);

        if (feof(stdin)) {
            printf("\n");
            close(client_socket);
            done = true;
            continue;
        }

        int msg_len = strlen(buf + 2);
        buf[0] = '\0';
        buf[1] = (char) msg_len;

        /* odeslani zpravy na server */
        bytestx = sendto(client_socket, buf, msg_len + 2, 0, (struct sockaddr *) &server_address, server_address_length);
        if (bytestx < 0) {
            perror("ERROR in sendto");
            continue;
        }
        bzero(buf, BUFSIZE);

        /* prijeti odpovedi a jeji vypsani */
        bytesrx = recvfrom(client_socket, buf, BUFSIZE, 0, (struct sockaddr *) &server_address, &server_address_length);
        if (bytesrx < 0) {
            perror("ERROR in recvfrom");
            continue;
        }

        printf("Echo from server: %s", buf + 2);
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

const struct option longopts[] =
  {
    {"host",      required_argument,  0, 'h'},
    {"port",      required_argument,  0, 'p'},
    {"mode",      required_argument,  0, 'm'},
    {0,0,0,0}
  };
    while ((arg = getopt_long(argc, argv, "h:p:m:", longopts, NULL))!=-1)
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


    if (host.empty() || port.empty() || mode.empty())
    {
        fprintf(stderr, "Usage: [-h host] [-p port] [-m mode]\n");
        return EXIT_FAILURE;
    }

    port_number = atoi(port.c_str());
    server_hostname = host.c_str();

    if ((server = gethostbyname(server_hostname)) == NULL) {
        fprintf(stderr,"ERROR: no such host as %s\n", server_hostname);
        exit(EXIT_FAILURE);
    }


    bzero((char *) &server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&server_address.sin_addr.s_addr, server->h_length);
    server_address.sin_port = htons(port_number);

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
        udpitis(client_socket, buf, server_address, endit, bytestx, bytesrx);
    }
    else
    {
        fprintf(stderr, "ERROR: wrong mode\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}