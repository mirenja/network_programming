#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <unistd.h>  

///this is entirely provided by chatgpt for testing:
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
    
    const int PORT = 8080;
    struct sockaddr_in address{};
    socklen_t addrlen = sizeof(address);
    int new_socket;

    // create socket file descriptor(fd)
    int server_fd = socket(AF_INET,SOCK_STREAM,0);
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
    //bind returns 0 on success and -1 on failure

    //bind or naming. associate it with the address
    if(bind(server_fd, reinterpret_cast<sockaddr*>(&address),sizeof(address)) < 0){
        std::cerr << "binding failed" << std::strerror(errno) << std::endl;
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    //listening and accept

    std::cout<< "Bind successful, server listening on port " << PORT << std::endl;
    if(listen(server_fd,128) < 0){
        std::cerr<< "Listen Error "  <<std::strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
  
    while(true){

        std::cout << "\n+++++++ Waiting for new connection ++++++++\n\n"<<std::endl;
        if((new_socket = accept(server_fd,reinterpret_cast<sockaddr*>(&address), &addrlen))< 0){
            std::cerr<< "Accept error "  <<std::strerror(errno) << std::endl; 
            exit(EXIT_FAILURE);

        }

        //send and receive messages
        constexpr unsigned size_of_buffer = 1024;
        char buffer[size_of_buffer] = {0};

        const char message[] = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
        const char *hello = message;
        // Read data from the client
        int valread = read(new_socket, buffer, size_of_buffer);
        if (valread < 0) {
            std::cerr << "Read error: " << std::strerror(errno) << std::endl;
        } else if (valread == 0) {
            std::cout << "Client disconnected" << std::endl;
        } else {
            std::cout << "Message received: " << buffer << std::endl;
        }
        // Send response to the client
        //const char *hello = "Hello from the server";  this doesnt make sense, check later pointing a pointer to a string not an address

        if (write(new_socket, hello, strlen(hello)) < 0) {
            std::cerr << "Write error: " << std::strerror(errno) << std::endl;
        } else {
            std::cout << "Message sent to client" << std::endl;
        }
        close(new_socket);

    }
    return 0;
}



 