#include "../include/PlaceOrder.h"
#include "../include/APIClient.h"
#include <iostream>
#include <jsoncpp/json/json.h>
#include <string>
#include <sstream> 
#include <string>  // For std::string
 // For std::istringstream


void PlaceOrder::place_order(const std::string& access_token) {
    try {
        // Check if the access token is valid
        if (api_client.get_token().empty()) {
            throw std::runtime_error("Authentication failed");
        }

        std::string direction, instrument_name, type, label;
        float amount, price; // Add a price variable

        // Get user input for placing the order
        std::cout << "Enter direction (buy/sell): ";
        std::cin >> direction;
        std::cout << "Enter instrument name (e.g., ETH-PERPETUAL): ";
        std::cin >> instrument_name;
        std::cout << "Enter amount: ";
        std::cin >> amount;
        std::cout << "Enter order type (e.g., market, limit): ";
        std::cin >> type;

        // Add input for price only if the order type is limit
        if (type == "limit") {
            std::cout << "Enter price: ";
            std::cin >> price; // Get price for limit order
        } else {
            price = 0; // Default value if it's a market order
        }

        std::cout << "Enter label (optional identifier for the order): ";
        std::cin >> label;

        // Construct the parameters string for the API request
        std::string params = "amount=" + std::to_string(amount) +
                             "&instrument_name=" + instrument_name +
                             "&label=" + label +
                             "&type=" + type;

        // Include price in parameters if it's a limit order
        if (type == "limit") {
            params += "&price=" + std::to_string(price);
        }

        // Make the request to place the order
        std::string response = api_client.make_request("private/" + direction, params, true);

        // Parse the JSON response
        Json::Value root;
        Json::CharReaderBuilder reader;
        std::string errors;
        std::istringstream response_stream(response);

        // Check if parsing was successful
        if (Json::parseFromStream(reader, response_stream, &root, &errors)) {
            // Output the parsed response
            std::cout << "Parsed Order Response: " << root << std::endl;
        } else {
            throw std::runtime_error("Failed to parse the JSON response: " + errors);
        }
    } catch (const std::exception& e) {
        // Handle exceptions and print error messages
        std::cerr << "Error placing order: " << e.what() << std::endl;
    }
}
