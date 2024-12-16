    #include "../include/DeribitAPI.h"
    #include "../include/APIClient.h"
    #include "../include/PlaceOrder.h"
    #include "../include/GetPositions.h"
    #include "../include/GetOrderBook.h"
    #include "../include/ModifyOrder.h"
    #include "../include/CancelOrder.h"
    #include <iostream>
    #include <jsoncpp/json/json.h>
    #include <memory>
    #include <string> 
    #include <sstream>  // For std::istringstream


    using namespace std::chrono;

    /**
     * @brief Constructs a DeribitAPI object, initializing base URL and credentials.
     */
    DeribitAPI::DeribitAPI()
        : BASE_URL("https://test.deribit.com/api/v2/"),
        CLIENT_ID("R9yBB9X6"),
        API_KEY("IP3CyQUNvYQupFaXrNhenKJ82IDNwSwtyaVDNbVdOXg"),
        token_expiration_time(900) { // Default token expiration time set to 900 seconds
    }

    /**
     * @brief Retrieves the current access token, refreshing it if necessary.
     * @return The current access token.
     */
    std::string DeribitAPI::get_token() {
        if (access_token.empty() || is_token_expired()) {
            refresh_token();
        }
        return access_token;
    }

    /**
     * @brief Places an order using the PlaceOrder class.
     */
    void DeribitAPI::place_order() {
        PlaceOrder order(*this);
        order.place_order(get_token());
    }

    /**
     * @brief Retrieves the current positions using the GetPositions class.
     */
    void DeribitAPI::get_positions() {
        GetPositions positions(*this);
        positions.get_positions(get_token());
    }

    /**
     * @brief Retrieves the order book using the GetOrderBook class.
     */
    void DeribitAPI::get_order_book() {
        GetOrderBook orderBook(*this);
        orderBook.get_order_book();
    }

    /**
     * @brief Modifies an existing order using the ModifyOrder class.
     */
    void DeribitAPI::modify_order() {
        ModifyOrder order(*this);
        order.modify_order(get_token());
    }

    /**
     * @brief Cancels an existing order using the CancelOrder class.
     */
    void DeribitAPI::cancel_order() {
        CancelOrder order(*this);
        order.cancel_order(get_token());
    }

    /**
     * @brief Checks if the current access token is expired.
     * @return true if the token is expired, false otherwise.
     */
    bool DeribitAPI::is_token_expired() const {
        steady_clock::time_point current_time = steady_clock::now();
        duration<double> elapsed_seconds = current_time - token_creation_time;
        return elapsed_seconds.count() >= token_expiration_time;
    }

    /**
     * @brief Refreshes the access token by making a request to the authentication endpoint.
     */
    void DeribitAPI::refresh_token() {
        std::string token_url = BASE_URL + "public/auth?client_id=" + CLIENT_ID +
                                    "&client_secret=" + API_KEY +
                                    "&grant_type=client_credentials";

        std::string response = make_request("public/auth",
            "client_id=" + CLIENT_ID +
            "&client_secret=" + API_KEY +
            "&grant_type=client_credentials", false);

        Json::Value root;
        Json::CharReaderBuilder reader;
        std::string errors;
        std::istringstream response_stream(response);

        if (Json::parseFromStream(reader, response_stream, &root, &errors)) {
            if (root["result"].isMember("access_token")) {
                access_token = root["result"]["access_token"].asString();
                token_creation_time = steady_clock::now();
                token_expiration_time = root["result"]["expires_in"].asInt();
                 std::cout << "New token acquired: " << access_token << std::endl;
            std::cout << "Token valid for " << token_expiration_time << " seconds." << std::endl;

            } else {
                std::cerr << "Failed to acquire token: " << root << std::endl;
            }
        } else {
            std::cerr << "Failed to parse the JSON response: " << errors << std::endl;
        }
    }

    /**
     * @brief Makes an API request to the Deribit server.
     * @param endpoint The API endpoint to request.
     * @param params The query parameters for the request.
     * @param auth Indicates whether authentication is required.
     * @return The response body from the server.
     * @throws std::runtime_error if the request fails.
     */
    std::string DeribitAPI::make_request(const std::string& endpoint, const std::string& params, bool auth) {
        std::string readBuffer;
        std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curl(curl_easy_init(), curl_easy_cleanup);

        if (curl) {
            std::string url = BASE_URL + endpoint + "?" + params;
            curl_easy_setopt(curl.get(), CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, APIClient::WriteCallback); // Use the WriteCallback method
            curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &readBuffer);

            std::unique_ptr<curl_slist, decltype(&curl_slist_free_all)> headers(nullptr, curl_slist_free_all);
            if (auth) {
                headers.reset(curl_slist_append(nullptr, ("Authorization: Bearer " + access_token).c_str()));
                curl_easy_setopt(curl.get(), CURLOPT_HTTPHEADER, headers.get());
            }

            CURLcode res = curl_easy_perform(curl.get());
            if (res != CURLE_OK) {
                throw std::runtime_error("CURL request failed: " + std::string(curl_easy_strerror(res)));
            }
        } else {
            throw std::runtime_error("Failed to initialize CURL");
        }

        return readBuffer;
    }
