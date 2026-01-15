#include "Socket.h"
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>


Socket::Socket() 
    : fd_(-1), bound_(false), listening_(false) 
{
     // create socket file descriptor(fd)
    fd_ = socket(AF_INET, SOCK_STREAM, 0);
    std::cout << "Socket created with FD = " << fd_ << std::endl;
    if (fd_ < 0) {
        std::cerr << "Cannot create socket\n";
        return;
    }

    // avoid "address already in use" when we restart and the port is in TIME WAIT 
    int opt = 1;
    setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
}

// private constructor for accepted clients
Socket::Socket(int fd, sockaddr_in addr) 
    : fd_(fd), address_(addr), bound_(true), listening_(false) {}

Socket::~Socket() {
    close();
}

//bind or naming. associate it with the address
// if there is an error fd will return -1
bool Socket::bind(int port) {
    if (fd_ < 0) return false;

    address_.sin_family = AF_INET;
    address_.sin_addr.s_addr = INADDR_ANY;
    address_.sin_port = htons(port);       

    if (::bind(fd_, reinterpret_cast<sockaddr*>(&address_), sizeof(address_)) < 0) {
        std::cerr << "binding failed" << std::strerror(errno) << "\n";
        return false;
    }

    bound_ = true;
    return true;
}

    

bool Socket::listen(int backlog) {
    if (!bound_) return false;

    if (::listen(fd_, backlog) < 0) {
        std::cerr << "Listen failed: " << std::strerror(errno) << "\n";
        return false;
    }

    listening_ = true;
    return true;
}


Socket Socket::accept() {
    if (!listening_) 
    return Socket(-1, sockaddr_in{});

    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
    int client_fd = ::accept(fd_, reinterpret_cast<sockaddr*>(&client_addr), &client_len);
    std::cerr << "passed fd_  " << fd_ << std::endl;
    std::cerr << "client fd_   " << client_fd << std::endl;

    if (client_fd < 0) {
        std::cerr << "Accept failed: " << std::strerror(errno) << "\n";
        return Socket(-1, sockaddr_in{});
    }

    return Socket(client_fd, client_addr);
}

ssize_t Socket::send(const std::string& data) {
    if (fd_ < 0) return -1;
    return ::send(fd_, data.c_str(), data.size(), 0);
}


std::string Socket::receive(size_t buffer_size) {
    if (fd_ < 0) return "";

    char buffer[buffer_size];
    ssize_t n = ::recv(fd_, buffer, buffer_size - 1, 0);
    std::cout << "receive returns= " << n << std::endl;

    if (n <= 0) return "";
    buffer[n] = '\0';
    return std::string(buffer, n);
}


void Socket::close() {
    if (fd_ >= 0) ::close(fd_);
    fd_ = -1;
    bound_ = false;
    listening_ = false;
}