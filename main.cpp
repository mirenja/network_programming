#include "HttpServer.h"
#include <iostream>
#include <csignal>

HttpServer* server_ptr = nullptr;

void signalHandler(int signal) {
    std::cout << "\nShutting down server..." << std::endl;
    if (server_ptr) {
        server_ptr->stop();
    }
    exit(0);
}

int main() {
    const int PORT = 8080;

    std::signal(SIGINT, signalHandler);
    
    try {
        HttpServer server(PORT);
        server_ptr = &server;
        
        std::cout << "Starting HTTP Server on port " << PORT << std::endl;
        std::cout << "Press Ctrl+C to stop\n" << std::endl;
        
        server.run();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}