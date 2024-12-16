#include "../include/HFTClient.h"
#include <curl/curl.h>

// Constructor for the HFTClient class.
// Initializes the resolver and WebSocket stream with the provided I/O context and SSL context.
// Starts the WebSocket connection as soon as the object is created.
HFTClient::HFTClient(net::io_context& ioc, ssl::context& ctx)
    : resolver_(ioc), ws_(ioc, ctx){}


// Starts the WebSocket client by resolving the specified host and port.
// Takes the instrument name as an argument for subscription purposes.
void HFTClient::run()
{
    host_ = "127.0.0.1"; // Store the host
    port_ = "8080";

    // Proceed with resolving the connection
    resolver_.async_resolve(host_, port_, beast::bind_front_handler(&HFTClient::on_resolve, shared_from_this()));
}

// Callback function for handling the result of the asynchronous resolve operation.
// If successful, it initiates a connection to the resolved endpoint.
void HFTClient::on_resolve(beast::error_code ec, tcp::resolver::results_type results)
{
    if (ec) // Check for errors
        return fail(ec, "resolve"); // Handle error

    // Set the timeout for the connection
    beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));
    // Asynchronously connect to the endpoint
    beast::get_lowest_layer(ws_).async_connect(results,
        beast::bind_front_handler(&HFTClient::on_connect, shared_from_this()));
}

// Callback function for handling the result of the asynchronous connect operation.
// If successful, it initiates the SSL handshake.
void HFTClient::on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep)
{
    if (ec) // Check for errors
        return fail(ec, "connect"); // Handle error

    host_ += ':' + std::to_string(ep.port()); // Store the port
    beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30)); // Set timeout
    // Asynchronously perform the SSL handshake
    ws_.next_layer().async_handshake(ssl::stream_base::client,
        beast::bind_front_handler(&HFTClient::on_ssl_handshake, shared_from_this()));
}

// Callback function for handling the result of the asynchronous SSL handshake.
// If successful, it initiates the WebSocket handshake.
void HFTClient::on_ssl_handshake(beast::error_code ec)
{
    if (ec) // Check for errors
        return fail(ec, "ssl_handshake"); // Handle error

    beast::get_lowest_layer(ws_).expires_never(); // No timeout for the WebSocket connection
    ws_.set_option(websocket::stream_base::timeout::suggested(beast::role_type::client)); // Set timeout options
    // Asynchronously perform the WebSocket handshake
    ws_.async_handshake(host_, "/ws/api/v2",
        beast::bind_front_handler(&HFTClient::on_handshake, shared_from_this()));
}

// Callback function for handling the result of the asynchronous WebSocket handshake.
// If successful, it subscribes to the order book.
void HFTClient::on_handshake(beast::error_code ec)
{
    if (ec) // Check for errors
        return fail(ec, "handshake"); // Handle error

    // Successful WebSocket handshake, can now send subscription message.
    std::cout << "WebSocket handshake successful, ready to subscribe!" << std::endl;
}

// Function to subscribe to an instrument (e.g., btc-perpetual).
void HFTClient::subscribe(const std::string& instrument)
{
    Json::Value msg;
    msg["method"] = "subscribe";
    msg["params"]["instrument"] = instrument;

    std::cout<<"Subscribing to "<<instrument<<std::endl;

    // Send the subscription message over the WebSocket.
    std::string message = msg.toStyledString();
    ws_.async_write(net::buffer(message),
        beast::bind_front_handler(&HFTClient::on_write, shared_from_this()));
}

// Function to unsubscribe from an instrument (e.g., btc-perpetual).
void HFTClient::unsubscribe(const std::string& instrument)
{
    Json::Value msg;
    msg["method"] = "unsubscribe";
    msg["params"]["instrument"] = instrument;
    std::cout<<"Unsubscribing to "<<instrument<<std::endl;

    // Send the unsubscribe message over the WebSocket.
    std::string message = msg.toStyledString();
    ws_.async_write(net::buffer(message),
        beast::bind_front_handler(&HFTClient::on_write, shared_from_this()));
}

// Callback function for handling the result of the asynchronous write operation.
// Initiates reading messages from the WebSocket after a successful write.
void HFTClient::on_write(beast::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred); // Ignore the number of bytes transferred

    if (ec) // Check for errors
        return fail(ec, "write"); // Handle error

    read(); // Start reading messages from the WebSocket
}

// Initiates an asynchronous read operation from the WebSocket.
void HFTClient::read()
{
    ws_.async_read(buffer_, // Start reading into the buffer
        beast::bind_front_handler(&HFTClient::on_read, shared_from_this()));
}

// Callback function for handling the result of the asynchronous read operation.
// Processes the received message and prints it to the console.
void HFTClient::on_read(beast::error_code ec, std::size_t bytes_transferred)
{
    std::cout<<"data reveving"<<std::endl;
    boost::ignore_unused(bytes_transferred); // Ignore the number of bytes transferred

    if (ec) // Check for errors
        return fail(ec, "read"); // Handle error

    std::string data = beast::buffers_to_string(buffer_.data()); // Convert the buffer to a string
    Json::Value root; // Create a JSON object to hold the parsed data
    Json::Reader reader; // Create a reader for parsing JSON
    if (reader.parse(data, root)) // Parse the JSON data
    {
        std::cout << "Received message:\n" << root.toStyledString() << std::endl; // Print the parsed message
    }
    else
    {
        std::cerr << "Failed to parse JSON: " << data << std::endl; // Log parse error
    }

    buffer_.consume(buffer_.size()); // Clear the buffer after processing
    read(); // Continue reading for more messages
}

// Handles errors by printing the error message to the console.
void HFTClient::fail(beast::error_code ec, char const* what)
{
    std::cerr << what << ": " << ec.message() << "\n"; // Log the error
}
