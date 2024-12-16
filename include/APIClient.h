#ifndef APICLIENT_H
#define APICLIENT_H

#include <string>
#include <curl/curl.h>

/**
 * @class APIClient
 * @brief A client class to manage API requests using libcurl.
 */
class APIClient {
public:
    /**
     * @brief Callback function to handle data received from the server.
     * @param contents Pointer to the data received.
     * @param size Size of each data element.
     * @param nmemb Number of data elements.
     * @param userp User-defined pointer to store the output data.
     * @return The total size of data processed.
     */
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);

    /**
     * @brief Makes an HTTP request to the specified URL with optional authentication.
     * @param url The URL to which the request is made.
     * @param params The parameters to include in the request body.
     * @param auth A boolean flag indicating if authorization is required.
     * @param token The authorization token to include in the request header if auth is true.
     * @return A string containing the server response.
     */
    static std::string make_request(const std::string& url, const std::string& params, bool auth, const std::string& token);
};

#endif // APICLIENT_H
