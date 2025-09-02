#include<iostream>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <unistd.h>  


int main()
{
    /*
        CREATE THE SOCKET
        A file descriptor is just a small integer that the kernel uses as a handle to track an open file or socket.
        server_fd is a file descriptor representing the listening socket.
        The 0 in socket(AF_INET, SOCK_STREAM, 0) means “use the default protocol for this domain and type”, which is TCP for stream sockets.
        In the protocol parameter, 1 usually corresponds to ICMP, raw socket with (no transport layer like TCP/UDP),ICMP reports on latenc, error,
        PING tests

        2. Identify the socket
        struct sockaddr_in 
                { 
                    __uint8_t         sin_len; 
                    sa_family_t       sin_family; 
                    in_port_t         sin_port; 
                    struct in_addr    sin_addr; 
                    char              sin_zero[8]; 
                };
        struct in_addr {
                uint32_t s_addr;  // 4-byte IPv4 address in network byte order
                };


3. On the server, wait for an incoming connection
4. Send and receive messages
5. Close the socket
    */
    int server_fd = socket(AF_INET,SOCK_STREAM,0);
    const int PORT = 8080;
    struct sockaddr_in address{};

    if (server_fd < 0){
        std::cerr << "cannot create socket\n";
    }

    //identify the socket
    address.sin_family =AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(PORT);

    //int bind(int socket, const struct sockaddr *address, socklen_t address_len);
    //reinterpret cast to say this is the address of or its a pointer to sockaddr type not sockaddr_in type as defined in the lib
    //print the system error, use std::strerror(errno) from <cerrno>
    if(bind(server_fd, reinterpret_cast<sockaddr*>(&address),sizeof(address)) < 0){
        std::cerr << "binding failed" << std::strerror(errno) << std::endl;
        close(server_fd);
        return 1;
    }
    std::cout<< "Bind successful, server listening on port " << PORT << std::endl;
    close(server_fd);
   

    return 0;
}