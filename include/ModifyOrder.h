#ifndef MODIFYORDER_H
#define MODIFYORDER_H

#include <string>
#include "../include/APIClient.h"
#include "../include/DeribitAPI.h"

/**
 * @class ModifyOrder
 * @brief A class to handle the modification of orders using the Deribit API.
 */
class ModifyOrder {
public:
    /**
     * @brief Constructor for ModifyOrder.
     * @param api_client A reference to an instance of DeribitAPI to interact with the API.
     */
    ModifyOrder(DeribitAPI& api_client);

    /**
     * @brief Modifies an existing order using the provided token.
     * @param token A string representing the order token for modification.
     */
    void modify_order(const std::string& token);

private:
    DeribitAPI api_client; ///< Instance of DeribitAPI used for making API requests.
};

#endif // MODIFYORDER_H
