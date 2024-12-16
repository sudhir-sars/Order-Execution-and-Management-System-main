#include "DeribitAPI.h"

/**
 * @class PlaceOrder
 * @brief A class to handle placing new orders through the Deribit API.
 */
class PlaceOrder {
public:
    /**
     * @brief Constructor for PlaceOrder.
     * @param api A reference to a DeribitAPI instance used to interact with the API.
     */
    PlaceOrder(DeribitAPI& api) : api_client(api) {}

    /**
     * @brief Places a new order using the provided token.
     * @param token A string representing the authorization token for placing the order.
     */
    void place_order(const std::string& token);

private:
    DeribitAPI& api_client; ///< Reference to the DeribitAPI instance for making API requests.
};
