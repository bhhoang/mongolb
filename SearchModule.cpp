#include <cstdint>
#include <iostream>
#include <vector>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

#define INFO_BUFFER_SIZE 32767
TCHAR  infoBuf[INFO_BUFFER_SIZE];
DWORD  bufCharCount = INFO_BUFFER_SIZE;

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;

mongocxx::instance instance{}; // This should be done only once.
mongocxx::uri uri("mongodb://localhost:27017");
mongocxx::client client(uri);

auto db = client["local"];

auto collection = db["Data"];

int main() {
    // Get and display the name of the computer.
    if (!GetComputerName(infoBuf, &bufCharCount))
        printError(TEXT("GetComputerName"));
    _tprintf(TEXT("\nComputer name:      %s"), infoBuf);

    // Get and display the user name.
    if (!GetUserName(infoBuf, &bufCharCount))
        printError(TEXT("GetUserName"));
    _tprintf(TEXT("\nUser name:          %s"), infoBuf);

    std::vector<std::string> options = { "name", "type", "brand", "price" };
    std::vector<std::string> chosenOptions;

    std::cout << "Choose the search options by entering the corresponding numbers: " << std::endl;
    for (int i = 0; i < options.size(); ++i) {
        std::cout << i + 1 << ". Search by " << options[i] << std::endl;
    }
    std::cout << "Enter 0 when you're done selecting options." << std::endl;

    int option;
    while (true) {
        std::cout << "Enter your choice (0 to finish): ";
        std::cin >> option;

        if (option == 0)
            break;

        if (option < 1 || option > options.size()) {
            std::cout << "Invalid option. Please choose a number between 1 and " << options.size() << "." << std::endl;
            continue;
        }

        std::string chosenOption = options[option - 1];

        auto it = std::find(chosenOptions.begin(), chosenOptions.end(), chosenOption);
        if (it == chosenOptions.end()) {
            chosenOptions.push_back(chosenOption);
        }
        else {
            chosenOptions.erase(it);
        }
    }

    if (chosenOptions.empty()) {
        std::cout << "You haven't chosen any options." << std::endl;
    }
    else {
        bsoncxx::builder::stream::document filter_builder{};
        std::cout << "You have chosen by ";
        for (size_t i = 0; i < chosenOptions.size(); ++i) {
            if (i != 0) {
                std::cout << " and ";
            }
            std::cout << chosenOptions[i];
        }
        std::cout << "." << std::endl;

        for (const auto& option : chosenOptions) {
            if (option == "name") {
                std::string name;
                std::cout << "Enter the name to search: ";
                std::cin >> name;
                filter_builder << "name" << name;
                // Perform search by name
            }
            else if (option == "type") {
                std::string type;
                std::cout << "Enter the type to search: ";
                std::cin >> type;
                filter_builder << "type" << type;
                // Perform search by type
            }
            else if (option == "brand") {
                std::string brand;
                std::cout << "Enter the brand to search: ";
                std::cin >> brand;
                filter_builder << "brand" << brand;
                // Perform search by brand
            }
            else if (option == "price") {
                double minPrice, maxPrice;
                std::cout << "Enter the minimum price: ";
                std::cin >> minPrice;
                std::cout << "Enter the maximum price: ";
                std::cin >> maxPrice;
                filter_builder << "price" << open_document << "$gte" << minPrice << "$lte" << maxPrice << close_document;
                // Perform search by price range
            }
        }
    }

    auto filter = filter_builder.view();

    auto cursor = collection.find(filter);

    if (cursor.begin() == cursor.end()) {
        std::cout << "No matching products found." << std::endl;
    } else {
        for (auto&& doc : cursor) {
            std::cout << bsoncxx::to_json(doc) << std::endl;
	    }
	}
    //If the search results are empty, display a message:"No matching products found."

    return 0;
}