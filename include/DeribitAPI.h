#ifndef DERIBITAPI_H
#define DERIBITAPI_H

#include <string>
#include <chrono>

/**
 * @class DeribitAPI
 * @brief A class to manage interactions with the Deribit API, including authentication, order management, and data retrieval.
 */
class DeribitAPI {
public:
    /**
     * @brief Constructor for DeribitAPI, initializes API settings and authenticates if necessary.
     */
    DeribitAPI();

    /**
     * @brief Retrieves a valid authentication token, refreshing it if expired.
     * @return The current access token as a string.
     */
    std::string get_token();

    /**
     * @brief Places a new order using the Deribit API.
     */
    void place_order();

    /**
     * @brief Retrieves the current positions held in the account.
     */
    void get_positions();

    /**
     * @brief Retrieves the current state of the order book for a specific market.
     */
    void get_order_book();

    /**
     * @brief Modifies an existing order.
     */
    void modify_order();

    /**
     * @brief Cancels an active order.
     */
    void cancel_order();

private:
    std::string access_token; ///< Current access token for API authorization.
    std::chrono::steady_clock::time_point token_creation_time; ///< Timestamp of token creation.
    int token_expiration_time; ///< Token expiration time in seconds.
    const std::string BASE_URL; ///< Base URL of the Deribit API.
    const std::string CLIENT_ID; ///< Client ID for API access.
    const std::string API_KEY; ///< API key for authentication.

    /**
     * @brief Checks if the current token is expired.
     * @return True if the token has expired, otherwise false.
     */
    bool is_token_expired() const;

    /**
     * @brief Refreshes the access token when it has expired.
     */
    void refresh_token();

    /**
     * @brief Sends an HTTP request to a specified endpoint with given parameters and optional authorization.
     * @param endpoint The API endpoint to request.
     * @param params Parameters for the request body.
     * @param auth Whether authorization is required for this request.
     * @return The server's response as a string.
     */
    std::string make_request(const std::string& endpoint, const std::string& params, bool auth);

    // Allow these classes to access private members for direct API interactions.
    friend class PlaceOrder;
    friend class GetPositions;
    friend class ModifyOrder;
    friend class CancelOrder;
};

#endif // DERIBITAPI_H
