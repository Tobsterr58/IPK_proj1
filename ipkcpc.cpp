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
                return EXIT_FAILURE;
                break;  
        }
    }

    cout << host << endl;
    cout << port << endl;
    cout << mode << endl;

}