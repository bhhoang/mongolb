#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "lib/json/single_include/nlohmann/json.hpp"
#include <fstream>
#include <cstdlib>
#include <pthread.h>

#define SIZE 1024
#define PORT 3000

static bool server_running = true;

void error(const char *msg){
    std::cerr << msg;
    std::exit(1);
}

int main(int argc, char **argv){
    struct sockaddr_in *server[10];
    struct sockaddr_in mongolb_addr, serv_addr, cli_addr;

    int clientsockfd;
    char *recv_buf;
    int last_server = 0;

    // Parse backend server list
    // Read file stream
    std::ifstream f("backendList.json");
    if (!f) {
        std::cerr << "Error reading file\n";
        return 1;
    }
    nlohmann::json j = nlohmann::json::parse(f);
    std::cout << j["servers"][0] << std::endl;

    int n_server = j["servers"].size();

    std::cout << "There are " << n_server << " servers" <<std::endl;
    int port = PORT;
    if (argc >= 2){
        port = std::atoi(argv[1]);
    }

    // Create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        error("Error creating socket!");
    }
    std::cout << "Successfully create socket!" << std::endl;

    mongolb_addr.sin_family = AF_INET;
    mongolb_addr.sin_addr.s_addr = INADDR_ANY;
    mongolb_addr.sin_port = htons(port);

    //bind
    int ret = bind(sockfd, (struct sockaddr *) &mongolb_addr, sizeof(mongolb_addr));
    if (ret < 0){
        error("Bind failed!");
    }
    std::cout << "Bind Successfully!" << std::endl;
    // Listen
    ret = listen(sockfd, 10);

    socklen_t cliLen = sizeof(cli_addr);
    // Accept

    while (server_running) {
        clientsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &cliLen);
        if (clientsockfd < 0) {
            std::cout << "Accept error!"<< std::endl;
        }
        std::cout << "Accept successfully!" << std::endl;
        recv_buf = (char *) malloc(SIZE);
        if (recv_buf == NULL){
            error("Error allocating memory!");
        }
        std::cout << "Allocated memory successfully!" << std::endl;
        // Read from client
        ret = recv(clientsockfd, recv_buf, SIZE, 0);
        if (ret < 0){
            error("Error reading from client!");
        }
        std::cout << "Read from client successfully!" << std::endl;
        std::cout << "Received message: " << recv_buf << std::endl;
        // Round robin decide which server to send to
        last_server = (last_server + 1) % n_server;
        std::cout << "Sending to server: " << last_server << std::endl;
          }

    return 0;
}
