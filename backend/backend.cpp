#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include <string>
#include <cstring>
#include <csignal>

#define MAXLINE 1024

int main(int argc, char **argv){
    if(argc < 2){
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        exit(EXIT_FAILURE);
    }
    int PORT = std::stoi(argv[1]);
    struct sockaddr_in servaddr;
    // Create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1){
        std::cerr << "Socket creation failed..." << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Socket created successfully..." << std::endl;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
    // Bind socket
    if((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0){
        std::cerr << "Socket bind failed..." << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Socket binded successfully..." << std::endl;
    // Listen
    if((listen(sockfd, 10)) != 0){
        std::cerr << "Listen failed..." << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Server listening on port " << PORT << "..." << std::endl;
    // Accept
    while(1){
        int connfd = accept(sockfd, (struct sockaddr*)NULL, NULL);
        if(connfd < 0){
            std::cerr << "Server accept failed..." << std::endl;
            exit(EXIT_FAILURE);
        }
        std::cout << "Server accept the client..." << std::endl;
        char *recv_buf = (char*)malloc(MAXLINE);
        if(recv_buf == NULL){
            std::cerr << "Error allocating memory..." << std::endl;
            exit(EXIT_FAILURE);
        }
        // Read from client
        int n = recv(connfd, recv_buf, MAXLINE, 0);
        if(n < 0){
            std::cerr << "Error reading from client..." << std::endl;
            exit(EXIT_FAILURE);
        }
        std::cout << "Read from client successfully..." << std::endl;
        std::cout << "Client: " << recv_buf << std::endl;
        // Send to client
        char send_buf[MAXLINE];
        if(send_buf == NULL){
            std::cerr << "Error allocating memory..." << std::endl;
            exit(EXIT_FAILURE);
        }
        strcpy(send_buf, "Hello from server");
        send(connfd, send_buf, strlen(send_buf), 0);
        std::cout << "Message sent to client..." << std::endl;
        free(recv_buf);
        close(connfd);
    }
    close(sockfd);
    return 0;
}
