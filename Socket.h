#pragma once

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

class Socket {
public:
    Socket();
    ~Socket();
    
    bool bind(int port);
    bool listen(int backlog = 128);
    Socket accept();

    ssize_t send(const std::string& data);
    std::string receive(size_t buffer_size = 4096);
    void close();
    bool isValid() const { return fd_ >= 0; }
    
private:
    Socket(int fd, sockaddr_in addr);
    
    int fd_;
    sockaddr_in address_ {}; 
    bool bound_;
    bool listening_;
};