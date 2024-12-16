#include "../include/CancelOrder.h"
#include "../include/APIClient.h"
#include <iostream>
#include <jsoncpp/json/json.h> // Include the JSON library for parsing responses
#include <sstream>    // For std::istringstream
#include <string> 

/**
 * @brief Constructs a CancelOrder object with a reference to an API client.
 * @param api_client A reference to the DeribitAPI object for making API calls.
 */
CancelOrder::CancelOrder(DeribitAPI& api_client) : api_client(api_client) {}

/**
 * @brief Cancels an existing order by ID using the DeribitAPI client.
 * This method requests user input for the order ID, sends a cancellation request, and parses the response.
 * @param access_token The authorization token required for API access.
 */
void CancelOrder::cancel_order(const std::string& access_token) {
    try {
        // Verify token validity
        if (api_client.get_token().empty()) {
            throw std::runtime_error("Authentication failed");
        }

        std::string order_id;

        // Prompt user for the order ID to cancel
        std::cout << "Enter order ID to cancel: ";
        std::cin >> order_id;

        // Prepare request parameters with the specified order ID
        std::string params = "order_id=" + order_id;

        // Make the request to cancel the order
        std::string response = api_client.make_request("private/cancel", params, true);

        // Output the response to the console
        std::cout << "Cancel order response: " << response << std::endl;

        // Parse JSON response
        Json::Value root;
        Json::CharReaderBuilder reader;
        std::string errors;
        std::istringstream response_stream(response);

        if (Json::parseFromStream(reader, response_stream, &root, &errors)) {
            std::cout << "Parsed Cancel Order Response: " << root << std::endl;
        } else {
            throw std::runtime_error("Failed to parse the JSON response: " + errors);
        }
    } catch (const std::exception& e) {
        // Handle and display any errors that occur during the cancellation process
        std::cerr << "Error canceling order: " << e.what() << std::endl;
    }
}
