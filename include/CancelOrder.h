#ifndef CANCELORDER_H
#define CANCELORDER_H

#include <string>
#include "../include/DeribitAPI.h"

/**
 * @class CancelOrder
 * @brief A class to handle order cancellation through the Deribit API.
 */
class CancelOrder {
public:
    /**
     * @brief Constructor for the CancelOrder class.
     * @param api_client A reference to an instance of DeribitAPI to interact with the API.
     */
    CancelOrder(DeribitAPI& api_client);

    /**
     * @brief Cancels an order using the provided token.
     * @param token A string representing the order token for cancellation.
     */
    void cancel_order(const std::string& token);

private:
    DeribitAPI api_client; ///< Instance of DeribitAPI to handle API requests.
};

#endif // CANCELORDER_H
