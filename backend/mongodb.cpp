#include <iostream>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

#define dataBaseName "local"
#define collectionName "Data"

int main() {
    mongocxx::instance instance{}; // This should be done only once.
    mongocxx::uri uri("mongodb://localhost:27017/");
    mongocxx::client client(uri);

    // Print out the connection status
    if(!client) {
        std::cout << "Failed to connect to the database" << std::endl;
        return 1;
    }
    std::cout << "Connected to the database" << std::endl;

    // List records in the collection
    auto collection = client[dataBaseName][collectionName];
    auto cursor = collection.find({});
    for(auto&& doc : cursor) {
        std::cout << bsoncxx::to_json(doc) << std::endl;
    }
}
