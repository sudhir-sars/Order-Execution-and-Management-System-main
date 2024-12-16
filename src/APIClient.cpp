#include "../include/APIClient.h"
#include <curl/curl.h>
#include <stdexcept>

/**
 * @brief Static callback function for handling data received from the CURL request.
 * This function appends the received data to a string passed in via `userp`.
 * @param contents Pointer to the received data.
 * @param size Size of each data unit.
 * @param nmemb Number of data units received.
 * @param userp Pointer to the user data, which is cast to a `std::string*` here.
 * @return Total number of bytes handled, which is `size * nmemb`.
 */
size_t APIClient::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    std::string* str = static_cast<std::string*>(userp);
    str->append(static_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}

/**
 * @brief Makes a HTTP GET request to a specified URL with optional authorization.
 * Initializes a CURL session, sets up the URL, and attaches headers if authorization is required.
 * @param url The base URL for the request.
 * @param params Query parameters to append to the URL.
 * @param auth Boolean indicating if authorization is needed.
 * @param token Authorization token, used if `auth` is true.
 * @return The response body as a string.
 * @throws std::runtime_error if CURL initialization or the request itself fails.
 */
std::string APIClient::make_request(const std::string& url, const std::string& params, bool auth, const std::string& token) {
    std::string readBuffer;
    CURL* curl = curl_easy_init();  // Initialize CURL session

    if (curl) {
        // Form the complete URL by appending parameters
        std::string full_url = url + "?" + params;
        curl_easy_setopt(curl, CURLOPT_URL, full_url.c_str());  // Set the URL
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, APIClient::WriteCallback);  // Set the callback function
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);  // Pass buffer to store data

        // Add authorization header if required
        if (auth) {
            struct curl_slist* headers = nullptr;
            headers = curl_slist_append(headers, ("Authorization: Bearer " + token).c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        }

        // Perform the CURL request and check for errors
        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);  // Clean up CURL session

        // Throw an error if the request failed
        if (res != CURLE_OK) {
            throw std::runtime_error("CURL request failed: " + std::string(curl_easy_strerror(res)));
        }
    } else {
        throw std::runtime_error("Failed to initialize CURL");
    }

    return readBuffer;  // Return the response
}
