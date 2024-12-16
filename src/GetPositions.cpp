#include "../include/GetPositions.h"
#include "../include/APIClient.h"
#include <iostream>
#include <jsoncpp/json/json.h> // Include the appropriate JSON library
#include <sstream>    // For std::istringstream
#include <string>     // For std::string


/**
 * @brief Retrieves and displays the current positions for a specified currency and kind.
 * @param access_token The access token for API authentication.
 */
void GetPositions::get_positions(const std::string& access_token) {
    try {
        // Check if the access token is valid
        if (api_client.get_token().empty()) {
            throw std::runtime_error("Authentication failed");
        }

        std::string currency, kind;

        // Prompt the user for the currency and kind
        std::cout << "Enter currency (e.g., BTC): ";
        std::cin >> currency;
        std::cout << "Enter kind (e.g., future): ";
        std::cin >> kind;

        // Construct the parameters for the API request
        std::string params = "currency=" + currency + "&kind=" + kind;

        // Make the API request to get positions
        std::string response = api_client.make_request("private/get_positions", params, true);

        // Parse the JSON response
        Json::Value root;
        Json::CharReaderBuilder reader;
        std::string errors;
        std::istringstream response_stream(response);

        // Check if the JSON response is parsed correctly
        if (Json::parseFromStream(reader, response_stream, &root, &errors)) {
            // Check if the result field is present
            if (root.isMember("result")) {
                std::cout << "Positions result: " << root["result"] << std::endl;
            } else {
                std::cout << "No result field found in the response." << std::endl;
            }
        } else {
            throw std::runtime_error("Failed to parse the JSON response: " + errors);
        }
    } catch (const std::exception& e) {
        // Handle any exceptions that occur during the process
        std::cerr << "Error getting positions: " << e.what() << std::endl;
    }
}
