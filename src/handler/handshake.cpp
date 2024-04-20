#include "handshake.h"
#include <cstdlib>
#include <pthread.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024

typedef struct {
    int client_sockfd;
    std::string message;
    nlohmann::json config;
} handshake_args;

void *handle_handshake(void *arg) {
    handshake_args *args = reinterpret_cast<handshake_args*>(arg);
    int client_sockfd = args->client_sockfd;
    int server_sockfd;
    struct sockaddr_in serv_addr;
    char cli_buf[BUFFER_SIZE], serv_buf[BUFFER_SIZE];
    int n, m;

    std::memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(args->config["port"].get<int>());
    serv_addr.sin_addr.s_addr = inet_addr(args->config["host"].get<std::string>().c_str());

    std::string message = args->message;

    std::cout << "Connecting to server " << args->config["host"].get<std::string>() << ":" << args->config["port"].get<int>() << std::endl;

    if ((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket() failed");
        pthread_exit(NULL);
    }

    if (connect(server_sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect() failed");
        close(server_sockfd);
        pthread_exit(NULL);
    }

    // Send message to server in config
    if (send(server_sockfd, message.c_str(), message.size(), 0) < 0) {
        perror("send() failed");
        close(server_sockfd);
        pthread_exit(NULL);
    }

    // Receive message from server
    if ((n = recv(server_sockfd, serv_buf, BUFFER_SIZE, 0)) < 0) {
        perror("recv() failed");
        close(server_sockfd);
        pthread_exit(NULL);
    }

    // Send response to client
    if (send(client_sockfd, serv_buf, n, 0) < 0) {
        perror("send() failed");
        close(server_sockfd);
        pthread_exit(NULL);
    }
    close(client_sockfd);
    close(server_sockfd);
    memset(cli_buf, 0, BUFFER_SIZE);
    memset(serv_buf, 0, BUFFER_SIZE);
    pthread_exit(NULL);
}
