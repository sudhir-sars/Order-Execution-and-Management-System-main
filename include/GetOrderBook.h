#ifndef GETORDERBOOK_H
#define GETORDERBOOK_H

#include "DeribitAPI.h"

/**
 * @class GetOrderBook
 * @brief A class to retrieve the current order book from the Deribit API.
 */
class GetOrderBook {
public:
    /**
     * @brief Constructor for GetOrderBook.
     * @param apiClient A reference to a DeribitAPI instance for making API requests.
     */
    GetOrderBook(DeribitAPI& apiClient);

    /**
     * @brief Fetches the current order book data from the API.
     */
    void get_order_book();

private:
    DeribitAPI& apiClient; ///< Reference to the DeribitAPI instance for accessing the API.
};

#endif // GETORDERBOOK_H
