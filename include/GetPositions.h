#ifndef GETPOSITIONS_H
#define GETPOSITIONS_H

#include <string>
#include "../include/DeribitAPI.h"

/**
 * @class GetPositions
 * @brief A class to retrieve the current account positions from the Deribit API.
 */
class GetPositions {
public:
    /**
     * @brief Constructor for GetPositions.
     * @param api_client A reference to an instance of DeribitAPI to make API requests.
     */
    GetPositions(DeribitAPI& api_client) : api_client(api_client) {}

    /**
     * @brief Fetches the current positions for the account using the provided token.
     * @param token The access token used for authenticating the API request.
     */
    void get_positions(const std::string& token);

private:
    DeribitAPI& api_client; ///< Reference to the DeribitAPI instance for accessing API methods.
};

#endif // GETPOSITIONS_H
