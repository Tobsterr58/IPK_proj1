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

#define BUFSIZE 1024

using namespace std;


int main(int argc, char *argv[])
{

int ch;
string host;
string port;
string mode;

    //while ((ch = getopt_long(argc, argv, "h:p:m:", longopts, NULL)) != -1)
    {
        switch (ch)
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
                return EXIT_FAILURE;
                break;  
        }
    }

    printf("host: %s port: %s mode: %s \n", host, port, mode);

}