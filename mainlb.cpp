#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "lib/json/single_include/nlohmann/json.hpp"
#include <fstream>

int main(){
    // Read file stream
    std::ifstream f("backendList.json");
    // Check if the file was read successfully
    if (!f) {
        std::cerr << "Error reading file\n";
        return 1;
    }
    // Parse the file into the json object
    nlohmann::json j = nlohmann::json::parse(f);
    std::cout << j["servers"][0] << std::endl;


    return 0;
}
