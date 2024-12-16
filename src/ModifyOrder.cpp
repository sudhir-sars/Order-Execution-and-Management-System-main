#include "../include/ModifyOrder.h"
#include "../include/APIClient.h"
#include "../include/DeribitAPI.h"
#include <iostream>
#include <jsoncpp/json/json.h> // Include the appropriate JSON library
#include <sstream>    // For std::istringstream
#include <string> 

ModifyOrder::ModifyOrder(DeribitAPI& api_client) : api_client(api_client) {}
void ModifyOrder::modify_order(const std::string& access_token) {
    try {
            if (api_client.get_token().empty()) {
                throw std::runtime_error("Authentication failed");
            }

            std::string order_id;
            int amount;
            double price;
            std::string advanced;

            std::cout << "Enter order ID: ";
            std::cin >> order_id;
            std::cout << "Enter amount: ";
            std::cin >> amount;
            std::cout << "Enter price: ";
            std::cin >> price;
            std::cout << "Enter advanced type (e.g., implv): ";
            std::cin >> advanced;

            std::string params = "order_id=" + order_id +
                                 "&amount=" + std::to_string(amount) +
                                 "&price=" + std::to_string(price);
            // Uncomment the following line if you want to include the advanced parameter
            // params += "&advanced=" + advanced;

            std::string response = api_client.make_request("private/edit", params, true);

            std::cout << "Modify order response: " << response << std::endl;

            Json::Value root;
            Json::CharReaderBuilder reader;
            std::string errors;
            std::istringstream response_stream(response);

            if (Json::parseFromStream(reader, response_stream, &root, &errors)) {
                std::cout << "Parsed Modify Order Response: " << root << std::endl;
            } else {
                throw std::runtime_error("Failed to parse the JSON response: " + errors);
            }
        } catch (const std::exception& e) {
            std::cerr << "Error modifying order: " << e.what() << std::endl;
        }
}
