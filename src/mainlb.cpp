#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../lib/json/single_include/nlohmann/json.hpp"
#include <fstream>
#include <cstdlib>
#include <pthread.h>
#include <string>
#include "handler/handshake.h"


#define SIZE 1024

static bool server_running = true;

void error(const char *msg){
    std::cerr << msg;
    std::exit(1);
}

typedef struct {
    int client_sockfd;
    int server_sockfd;
    std::string message;
    nlohmann::json config;
} handler_args;

int main(int argc, char **argv){
    struct sockaddr_in *server[10];
    struct sockaddr_in mongolb_addr, serv_addr, cli_addr;
    if (argc < 2){
        std::cerr << "Usage: " << argv[0] << " <port>\n";
        return 1;
    }
    int PORT = std::atoi(argv[1]);
    int clientsockfd;
    char *recv_buf, *send_buf;
    int last_server = 0;

    // Parse backend server list
    // Read file stream
    std::ifstream f("backendList.json");
    if (!f) {
        std::cerr << "Error reading file\n";
        return 1;
    }
    nlohmann::json j = nlohmann::json::parse(f);
    // std::cout << j["servers"][0] << std::endl;

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
        std::cout << ret << std::endl;
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
        std::cout << "Got connection from " << inet_ntoa(cli_addr.sin_addr) << ":" << ntohs(cli_addr.sin_port) << std::endl;
        std::cout << "Client socket: " << clientsockfd << std::endl;

        // Allocate memory
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
        // std::cout << "Read from client successfully!" << std::endl;
        std::cout << "Received message: " << recv_buf << std::endl;
        std::string message = recv_buf;
        std::cout << "Type of message: " << message << std::endl;
        // Round robin decide which server to send to
        if (last_server == n_server - 1){
            last_server = 0;
        } else {
            last_server++;
        }
        last_server = last_server % n_server;
        std::cout << "Sending to server: " << last_server << std::endl;
        handler_args *args = (handler_args *) malloc(sizeof(handler_args));
        args->client_sockfd = clientsockfd;
        args->message = message;
        args->config["port"] = j["servers"][last_server]["port"];
        args->config["host"] = j["servers"][last_server]["host"];
        args->config["client_ip"] = inet_ntoa(cli_addr.sin_addr);
        args->config["client_port"] = ntohs(cli_addr.sin_port);
        // Check if all the elements are not null
        for (nlohmann::json::iterator it = args->config.begin(); it != args->config.end(); ++it){
            if (it.value().is_null()){
                std::cerr << "Error: " << it.key() << " is null\n";
                return 1;
            }
        }
        // std::cout << "Config: " << args->config << std::endl;

        // Handle connection in a new thread
        pthread_t handler_thread;
        if (pthread_create(&handler_thread, NULL, handle_handshake, (void *) args) != 0){
            std::cerr << "Error creating thread\n";
        }
        pthread_detach(handler_thread);
        memset(recv_buf, 0, SIZE);
        // Reset args
        args = NULL;
   }

    close(clientsockfd);
    // free(recv_buf);
    close(sockfd);
    return 0; 
}
