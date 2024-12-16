#ifndef HFTCLIENT_H
#define HFTCLIENT_H

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/ssl.hpp>
#include <jsoncpp/json/json.h>
#include <cstdlib>
#include <string>
#include <memory>
#include <iostream>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;

/**
 * @class WebSocketClient
 * @brief Manages a WebSocket connection for real-time communication with a server, handling SSL/TLS encryption.
 */
class HFTClient : public std::enable_shared_from_this<HFTClient> {
public:
    /**
     * @brief Constructs a WebSocketClient instance.
     * @param ioc Reference to the I/O context for managing asynchronous operations.
     * @param ctx Reference to the SSL context for managing SSL/TLS handshakes.
     */
    HFTClient(net::io_context& ioc, ssl::context& ctx);

    /**
     * @brief Initiates the WebSocket connection process.
     * @param host The server hostname.
     * @param port The server port number.
     * @param instrument The financial instrument or data stream to subscribe to.
     */
    void run();

    /**
     * @brief Sends a subscription request for a financial instrument.
     * @param instrument The financial instrument to subscribe to.
     */
    void subscribe(const std::string& instrument);

    /**
     * @brief Sends an unsubscription request for a financial instrument.
     * @param instrument The financial instrument to unsubscribe from.
     */
    void unsubscribe(const std::string& instrument);

private:
    /**
     * @brief Resolves the host name to an IP address.
     * @param ec Error code for capturing any resolution errors.
     * @param results The resolved IP address and port.
     */
    void on_resolve(beast::error_code ec, tcp::resolver::results_type results);

    /**
     * @brief Connects to the resolved endpoint.
     * @param ec Error code for capturing any connection errors.
     * @param ep Endpoint type specifying the IP address and port.
     */
    void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep);


    
    /**
     * @brief Performs an SSL handshake with the server.
     * @param ec Error code for capturing any SSL handshake errors.
     */
    void on_ssl_handshake(beast::error_code ec);

    /**
     * @brief Initiates the WebSocket handshake after SSL handshake.
     * @param ec Error code for capturing any WebSocket handshake errors.
     */
    void on_handshake(beast::error_code ec);

    /**
     * @brief Handles the WebSocket message write operation.
     * @param ec Error code for capturing any write errors.
     * @param bytes_transferred The number of bytes written.
     */
    void on_write(beast::error_code ec, std::size_t bytes_transferred);

    /**
     * @brief Reads messages from the WebSocket stream.
     * @param ec Error code for capturing any read errors.
     * @param bytes_transferred The number of bytes read.
     */
    void on_read(beast::error_code ec, std::size_t bytes_transferred);


    /**
     * @brief Initiates a read operation to receive messages from the server.
     */
    void read();

    /**
     * @brief Handles any errors that occur, logging them for debugging purposes.
     * @param ec Error code capturing the type of error.
     * @param what A description of the operation that failed.
     */
    void fail(beast::error_code ec, char const* what);

    /**
     * @brief Constructs a JSON message for subscription or unsubscription.
     * @param method The method name ("subscribe" or "unsubscribe").
     * @param instrument The instrument to include in the message.
     * @return The constructed JSON string.
     */
    

    net::io_context ioc_; // I/O context
    ssl::context ctx_{ssl::context::tlsv12_client}; // SSL context
   
    

    
    tcp::resolver resolver_; ///< Resolves hostnames to IP addresses.
    websocket::stream<beast::ssl_stream<beast::tcp_stream>> ws_; ///< WebSocket stream with SSL support.
    beast::flat_buffer buffer_; ///< Buffer for storing incoming WebSocket messages.
    std::string host_; ///< Hostname of the WebSocket server.
    std::string port_; ///< Hostname of the WebSocket server.
    std::string instrument_; ///< Financial instrument or data stream for subscription.
    std::vector<std::string> instruments_; ///< List of active instruments to subscribe to.
};

#endif // HFTCLIENT_H
