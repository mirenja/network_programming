#include <iostream>
#include <cstring>      // for memset, strlen
#include <sys/socket.h>
#include <arpa/inet.h>  // for inet_pton
#include <unistd.h>     // for read, write, close
#include <cstdlib>      // for EXIT_FAILURE

///this is entirely provided by chatgpt for testing:
constexpr int PORT = 8080;

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr{};
    const char* message = "Hello from client";
    char buffer[1024] = {0};

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error\n";
        return EXIT_FAILURE;
    }

    // Prepare server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 address from text to binary
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address / Address not supported\n";
        return EXIT_FAILURE;
    }

    // Connect to server
    if (connect(sock, reinterpret_cast<struct sockaddr*>(&serv_addr), sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed\n";
        return EXIT_FAILURE;
    }

    // Send message
    if (send(sock, message, std::strlen(message), 0) < 0) {
        std::cerr << "Send failed\n";
        return EXIT_FAILURE;
    }
    std::cout << "Hello message sent\n";

    // Receive response
    ssize_t valread = read(sock, buffer, sizeof(buffer) - 1);
    if (valread < 0) {
        std::cerr << "Read failed\n";
        return EXIT_FAILURE;
    }

    buffer[valread] = '\0'; // Ensure null-terminated string
    std::cout << "Server response: " << buffer << std::endl;

    close(sock);
    return 0;
}
