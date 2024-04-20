#include <iostream>
#include <string>
#include <cstring> // For std::strerror()
#include <sys/socket.h>
#include <arpa/inet.h> // For inet_addr()
#include <unistd.h> // For close()
int main(int argc, char* argv[]) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        std::cerr << "Could not create socket: " << std::strerror(errno) << '\n';
        return 1;
    }
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <port> <server ip<\n";
        return 1;
    }
    int port = std::stoi(argv[1]);
    char *server_ip = argv[2];
    if (server_ip == nullptr) {
        server_ip = "127.0.0.1"; // Default to localhost"
    }
    sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(server_ip); // Server IP
    server.sin_family = AF_INET;
    server.sin_port = htons(port); // Server port

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        std::cerr << "Connect failed: " << std::strerror(errno) << '\n';
        return 1;
    }

    std::cout << "Connected\n";

    std::string message;
    std::cout << "Enter message: ";
    std::getline(std::cin, message);

    if (send(sock, message.c_str(), message.size(), 0) < 0) {
        std::cerr << "Send failed: " << std::strerror(errno) << '\n';
        return 1;
    }

    char buffer[1024] = {0};
    ssize_t recv_size = recv(sock, buffer, 1024, 0);
    if (recv_size == -1) {
        std::cerr << "recv failed: " << std::strerror(errno) << '\n';
        return 1;
    }

    std::cout << "Server reply: " << buffer << '\n';

    close(sock);

    return 0;
}

