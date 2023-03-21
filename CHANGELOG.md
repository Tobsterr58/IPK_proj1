## This program is a client application that can send and receive data to/from a server using TCP or UDP protocol based on the specified command line arguments.

### **Implemented functionality:**

    - Parses command line arguments using getopt_long.
    - Validates and converts the host and port arguments to the appropriate data types.
    - Connects to the server using TCP or UDP protocol based on the specified mode.
    - Sends and receives data to/from the server based on the chosen protocol.

### **Known limitations:**

    - Only supports IPv4 (AF_INET) and cannot work with IPv6 (AF_INET6).
    - Can not support upper cases in argument (force you to make it lower)
    - Does not check if port is valid only connect error will apear after trying to connect to server