# IPK_proj1

<div style="font-size:20px">

My solution for first IPK project - Calculator Protocol

xstect00 (Tobiáš Štec) </div>

## TCP
TCP is a widely used protocol for transmitting data over networks. It is a connection-oriented protocol, which means that before data is transmitted, the communicating devices must establish a connection and close it once the data transmission is complete.

One of the key features of TCP is its reliability in ensuring data is delivered to its intended destination. It uses extensive error-checking mechanisms, including flow control and data acknowledgment, which helps to prevent data loss during transmission.

While TCP's reliability is a significant advantage, it can result in slower transmission speeds compared to protocols such as UDP, which prioritize speed over reliability. Therefore, when selecting a protocol for data transmission, it is essential to consider the specific needs of the application and balance the trade-offs between speed and reliability.

## UDP
UDP is a Datagram-oriented protocol which means it does not have any overhead for establishing, maintaining or terminating connections. This makes it efficient for broadcasting and multicasting data on networks. However, the delivery of data to the destination cannot be guaranteed in UDP as it does not provide any reliability mechanism.

Unlike TCP, UDP provides only the basic error checking mechanism using checksums. Despite this limitation, UDP is considered faster, simpler and more efficient than TCP.

## Implementation

My implementation contain just one program `ipkcpc.cpp` with functions : 

- `main()` 

Main function starts by checking number of arguments and than control if all arguments are valid by `getopt_long()` and also `.empty()` to check if arguments are not empty. Then it converts port_number and server_hostname to right type. After checking program will continue by setting ports, server adress and check what mode was selected from `TCP` or `UDP` and write error if you choose other option.

- `tcp_it_is()`

Is function called by main function if user selected TCP mode. This function contain while loop until user send "BYE" to server or server respond with "BYE" somehow (wrong input for example). When CTRL+C is pressed by calling funtion `handle_sigint_TCP()` it send "BYE" to server to end connection and to recieve "BYE" from server. CTRL+D (EOF) is handled by `fgets()`. If its equal to NULL than we know that CTRL+D was pressed so it also called `handle_sigint_TCP()` function.

- `udp_it_is()`

Is function called by main function if user selected UDP mode. Differences from TCP called function are that for CTRL+C or CTRL+D this function use different functions. For CTRL+C it is : `handle_sigint_UDP` which just simply end the connection and for CTRL+D it uses `feof()` with standart input and also just simply close the connection and set done to `true` to end while loop. Also UDP mode must check what is responded by server. First byte of sent message must be filled by 0 and first recieved byte by 1 (OPCODE). Second byte of sent message is filled with binary representation of calculated length of our message that we want to send. Second byte (counting from 1) of recieved message is status code 0 = OK and 1=ERR. And third byte is lenght of recieved message. Other bytes are for message. 

Checking server hostname: 

``` c++
if ((server = gethostbyname(server_hostname)) == NULL) { 
        fprintf(stderr,"ERROR: no such host as %s\n", server_hostname);
        return EXIT_FAILURE;
    }
```

Server adress is set by this code :
``` c++
bzero((char *) &server_address, sizeof(server_address)); //clears server_adress
    server_address.sin_family = AF_INET; //sets server_address to AF_INET (IPv4)
    bcopy((char *)server->h_addr, (char *)&server_address.sin_addr.s_addr, server->h_length); 
    server_address.sin_port = htons(port_number); //sets port number to server_address
```

Code for function for TCP to send "BYE" to server (signal handler) and also recieve "BYE" and than close connection.
``` c++
void handle_sigint_TCP(int sig) {
    send(client_socket, "BYE\n", strlen("BYE\n"), 0);
    char buf[BUFSIZE];
    bzero(buf, BUFSIZE);
    recv(client_socket, buf, BUFSIZE, 0);
    printf("%s", buf);
    close(sig);
    exit(0);
}
```

And for UDP signal handling it is this function which just close the connection
``` c++
void handle_sigint_UDP(int sig) {
    close(sig);
    exit(0);
}
```

## <span style="color: red "> <u>TESTS</u> </span>

### <div style="color: lightblue">**ARGUMENT TESTS**</div>


### - unknown flag
    
    Input:

    ./ipkcpc -h localhost -p 2023 -n idk

    Output:

    Error:: invalid option -- 'n'
    Usage: [-h host] [-p port] [-m mode]

### - in different order

    Input:

    ./ipkcpc -h localhost -p 2023 -m udp

    Output:

    **starts and runs the program**
### - case insensitive

    Input:

    ./ipkcpc -H localhost -p 2023 -M Udp

    Output:
    
    Error: Usage: [-h host] [-p port] [-m mode]



### - invalid mode
        
    Input:

    ./ipkcpc -H localhost -p 125 -m IP

    Output:

    ERROR: wrong mode (TCP or UDP)

### - missing argument
    
    Input:
    ./ipkcpc -h localhost -p 2023

    Output:

    Error: Usage: [-h host] [-p port] [-m mode]




<table border="1">
 <tr>
    <td><b style="font-size:20px">Tests mode - TCP</b></td>
    <td><b style="font-size:20px">Tests mode - UDP</b></td>
 </tr>
 <tr>
    <td>
    <span style="display: inline-block; width:400px">

### <div style="color: #bada55">**TEST 1**</div>
**Input :**

HELLO

SOLVE (+ 3 4)

BYE

**Expected :**

HELLO

RESULT 7

BYE

**Got :**

HELLO

RESULT 7

BYE

### <div style="color: #bada55">**TEST 2**</div>
**Input :**

HELLO

BYE

**Expected :**

HELLO

BYE

**Got :**

HELLO

BYE

### <div style="color: #bada55">**TEST 3**</div>
**Input :**

random

**Expected :**

BYE

**Got :**

BYE

### <div style="color: #bada55">**TEST 4**</div>
**Input :**

HELLO

CTRL+C

**Expected :**

HELLO

BYE

**Got :**

HELLO

BYE

### <div style="color: #bada55">**TEST 5**</div>
**Input :**

CTRL+D

**Expected :**

BYE

**Got :**

BYE

**✅Test 1 Passed**

**✅Test 2 Passed**

**✅Test 3 Passed**

**✅Test 4 Passed**

**✅Test 5 Passed**
    </span></td>
    <td>
    <span style="display: inline-block; width:400px">
### <div style="color: #bada55">**TEST 1**</div>
**Input :**

(- 6 4)

**Expected :**

OK:2

**Got :**

OK:2

### <div style="color: #bada55">**TEST 2**</div>
**Input :**

HELLO

**Expected :**

ERR:`<error message>`

**Got :**

`ERR:Invalid expression: h e l l o`

### <div style="color: #bada55">**TEST 3**</div>
**Input :**

random

(* 2 5)

**Expected :**

ERR:`<error message>`

OK:10

**Got :**

ERR:`Invalid expression: r a n d o m`

OK:10

### <div style="color: #bada55">**TEST 4**</div>
**Input :**

CTRL+C

**Expected :**

`close connection`

**Got :**


### <div style="color: #bada55">**TEST 5**</div>
**Input :**

CTRL+D

**Expected :**

`close connection`

**Got :**


**✅Test 1 Passed**

**✅Test 2 Passed**

**✅Test 3 Passed**

**✅Test 4 Passed**

**✅Test 5 Passed**</span></td>
 </tr>
</table>

## Extra

- UDP and TCP mode can be user as lower or upper case
- Flags do not need to be in order and are case insensitive: `./ipkcpc -m UDP -h localhost -p 2023`.
- In TCP mode also `bye` works as regular `BYE`

## Links that I ussed for my work

https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Stubs/cpp

https://moodle.vut.cz/mod/folder/view.php?id=289124

https://www.geeksforgeeks.org/differences-between-tcp-and-udp/

https://www.diffen.com/difference/TCP_vs_UDP
