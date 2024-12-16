#include "../include/GetOrderBook.h"
#include "../include/APIClient.h"
#include <iostream>
#include <jsoncpp/json/json.h>
#include <string>
#include <curl/curl.h>
#include <sstream>    // For std::istringstream
#include <string>     // For std::string

using namespace std;

/**
 * @brief Constructs a GetOrderBook object.
 * @param apiClient Reference to the DeribitAPI client for making requests.
 */
GetOrderBook::GetOrderBook(DeribitAPI& apiClient) : apiClient(apiClient) {}

/**
 * @brief Retrieves and displays the order book for a specified instrument.
 */
void GetOrderBook::get_order_book() {
    string instrument_name;
    int depth;

    // Prompt the user for the instrument name and depth
    cout << "Enter instrument name (e.g., BTC-PERPETUAL): ";
    cin >> instrument_name;
    cout << "Enter depth (e.g., 5): ";
    cin >> depth;

    // Construct the API request URL
    string order_book_url = "https://test.deribit.com/api/v2/public/get_order_book?instrument_name=" + instrument_name + "&depth=" + to_string(depth);

    CURL* curl;
    CURLcode res;
    string readBuffer;

    // Initialize CURL and perform the request
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, order_book_url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, APIClient::WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Perform the request
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl); // Cleanup CURL resources
    }

    // Output the raw response from the API
    cout << "Order book response: " << readBuffer << endl;

    // Parse the JSON response
    Json::CharReaderBuilder reader;
    Json::Value root;
    string errors;

    std::istringstream s(readBuffer);
    if (Json::parseFromStream(reader, s, &root, &errors)) {
        cout << "Parsed JSON Order Book: " << root << endl;
    } else {
        cout << "Failed to parse the JSON response: " << errors << endl;
    }
}
