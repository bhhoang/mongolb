#include <iostream>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

int main() {
    mongocxx::instance instance{}; // This should be done only once.
    mongocxx::uri uri("mongodb+srv://bhhoang:Ilove1000@nanikaclustor.mlolje2.mongodb.net/");
    mongocxx::client client(uri);

    // Print out the connection status
    if(!client) {
        std::cout << "Failed to connect to the database" << std::endl;
        return 1;
    }
    std::cout << "Connected to the database" << std::endl;

    // Print all the databases 
    auto databases = client.list_databases();
    for(auto&& db : databases) {
        std::cout << db["name"].get_string().value << std::endl;
    }
}
