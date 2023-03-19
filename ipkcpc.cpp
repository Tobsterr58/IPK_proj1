#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <getopt.h>

#define BUFSIZE 1024

using namespace std;


int main(int argc, char *argv[])
{

int arg=0;
string host;
string port;
string mode;
int client_socket, port_number, bytestx, bytesrx;
const char *server_hostname;
struct hostent *server;
struct sockaddr_in server_address;
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
        fprintf(stderr, "Usage: [-h host] [-p port] [-m mode] \n");
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

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
	{
		perror("ERROR: socket");
		exit(EXIT_FAILURE);
	}

    bzero(buf, BUFSIZE);
    printf("Please enter msg: ");
    fgets(buf, BUFSIZE, stdin);

    if (connect(client_socket, (const struct sockaddr *) &server_address, sizeof(server_address)) != 0)
    {
		perror("ERROR: connect");
		exit(EXIT_FAILURE);        
    }

    /* odeslani zpravy na server */
    bytestx = send(client_socket, buf, strlen(buf), 0);
    if (bytestx < 0) 
      perror("ERROR in sendto");
    
    /* prijeti odpovedi a jeji vypsani */
    bytesrx = recv(client_socket, buf, BUFSIZE, 0);
    if (bytesrx < 0) 
      perror("ERROR in recvfrom");
      
    printf("Echo from server: %s", buf);
        
    close(client_socket);
    return 0;
}