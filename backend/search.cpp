#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <string>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <vector>

#define MAXLINE 1024
#define DATABASE "local"
#define COLLECTION "Data"


std::string search(mongocxx::client& client, std::string searchValue) {
    auto collection = client[DATABASE][COLLECTION];
    auto cursor = collection.find(bsoncxx::builder::stream::document{} << "brand" << searchValue << bsoncxx::builder::stream::finalize);
    std::vector<std::string> results;
    for(auto&& doc : cursor) {
        results.push_back(bsoncxx::to_json(doc));
    }
    // Return the list as string
    std::string result;
    for (const auto& res : results) {
        result += res + "\n";
    }
    return result;
}


int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <port-for-host> <port-for-mongodb>\n";
        return EXIT_FAILURE;
    }

    int PORT = std::atoi(argv[1]);
    std::string MONGO_URI = "mongodb://localhost:" + std::string(argv[2]) + "/";
    sockaddr_in servaddr;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        std::cerr << "Socket creation failed...\n";
        return EXIT_FAILURE;
    }

    std::cout << "Socket created successfully...\n";
    std::memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if (bind(sockfd, reinterpret_cast<sockaddr*>(&servaddr), sizeof(servaddr)) != 0) {
        std::cerr << "Socket bind failed...\n";
        return EXIT_FAILURE;
    }

    std::cout << "Socket binded successfully...\n";
    if (listen(sockfd, 10) != 0) {
        std::cerr << "Listen failed...\n";
        return EXIT_FAILURE;
    }

    std::cout << "Server listening on port " << PORT << "...\n";
    mongocxx::instance instance{};
    mongocxx::uri uri(MONGO_URI);
    mongocxx::client client(uri);

    if (!client) {
        std::cerr << "Failed to connect to the database\n";
        return EXIT_FAILURE;
    }
    std::cout << "Connected to the database\n";

    while (true) {
        int connfd = accept(sockfd, nullptr, nullptr);
        if (connfd < 0) {
            std::cerr << "Server accept failed...\n";
            continue;  // Continue accepting other connections
        }
        std::cout << "Server accepted the client...\n";

        std::vector<char> recv_buf(MAXLINE);
        int n = recv(connfd, recv_buf.data(), MAXLINE, 0);
        if (n <= 0) {
            std::cerr << "Error reading from client or connection closed...\n";
            close(connfd);
            continue;
        }

        std::cout << "Read from client successfully...\n";
        std::string value(recv_buf.begin(), recv_buf.begin() + n);
        std::string results = search(client, value);
        std::string message = "Search results for " + value + ":\n" + results;
        std::cout << message << std::endl;
        if (send(connfd, results.c_str(), results.size(), 0) < 0) {
            std::cerr << "Error sending to client...\n";
        } else {
            std::cout << "Message sent to client...\n";
        }

        close(connfd);
    }

    close(sockfd);
    return 0;
}
