#include "../include/WebSocket.h"
#include <iostream>
#include <curl/curl.h>
#include "../include/ClientSession.h"
#include <jsoncpp/json/json.h>

// Constructor for Deribit client functionality
WebSocket::WebSocket(net::io_context& derbit_ioc,
                     ssl::context& derbit_ssl_ctx,
                     net::io_context& server_ioc,
                     ssl::context& server_ssl_ctx,
                     tcp::endpoint server_endpoint)
    : derbit_oc_(derbit_ioc),
      derbit_ssl_ctx_(derbit_ssl_ctx),
      derbit_resolver_(derbit_ioc),
      derbit_ws_(derbit_ioc, derbit_ssl_ctx),
      server_ioc_(server_ioc),
      server_ssl_ctx_(server_ssl_ctx),
      server_acceptor_(server_ioc, server_endpoint) {}
      



void WebSocket::derbitRunClient(const std::string& host, const std::string& port, const std::string& instrument) {
    derbit_host_ = host;
    derbit_instrument_ = instrument;
    derbit_resolver_.async_resolve(host, port, beast::bind_front_handler(&WebSocket::derbitOnResolve, shared_from_this()));
}

void WebSocket::derbitOnResolve(beast::error_code ec, tcp::resolver::results_type results) {
    if (ec) return derbitHandleError(ec, "resolve");
    beast::get_lowest_layer(derbit_ws_).expires_after(std::chrono::seconds(30));
    beast::get_lowest_layer(derbit_ws_).async_connect(results, beast::bind_front_handler(&WebSocket::derbitOnConnect, shared_from_this()));
}

void WebSocket::derbitOnConnect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep) {
    if (ec) return derbitHandleError(ec, "connect");
    beast::get_lowest_layer(derbit_ws_).expires_after(std::chrono::seconds(30));
    derbit_ws_.next_layer().async_handshake(ssl::stream_base::client, beast::bind_front_handler(&WebSocket::derbitOnSSLHandshake, shared_from_this()));
}

void WebSocket::derbitOnSSLHandshake(beast::error_code ec) {
    if (ec) return derbitHandleError(ec, "ssl_handshake");
    beast::get_lowest_layer(derbit_ws_).expires_never();
    derbit_ws_.set_option(websocket::stream_base::timeout::suggested(beast::role_type::client));
    derbit_ws_.async_handshake(derbit_host_, "/ws/api/v2", beast::bind_front_handler(&WebSocket::derbitOnHandshake, shared_from_this()));
}

void WebSocket::derbitOnHandshake(beast::error_code ec) {
    if (ec) return derbitHandleError(ec, "handshake");
    derbitSubscribeFromOrderBook("BTC-PERPETUAL");
}

void WebSocket::derbitSubscribeFromOrderBook(const std::string& symbol)
{
    Json::Value subscription; // Create a JSON object for the subscription message
    subscription["jsonrpc"] = "2.0"; // Set the JSON-RPC version
    subscription["id"] = 1; // Set the request ID
    subscription["method"] = "public/subscribe"; // Specify the method for subscription
    // Append the channel for the specified instrument to the parameters
    subscription["params"]["channels"].append("book." + symbol + ".100ms");

    Json::FastWriter writer; // Create a writer to convert JSON to a string
    std::string message = writer.write(subscription); // Write the JSON message to a string

    // Asynchronously send the subscription message
    derbit_ws_.async_write(net::buffer(message),
        beast::bind_front_handler(&WebSocket::derbitOnWrite, shared_from_this()));
}

void WebSocket::derbitUnsubscribeFromOrderBook(const std::string& symbol) {
Json::Value subscription;
    subscription["method"] = "subscribe";
    subscription["jsonrpc"] = "2.0"; // Set the JSON-RPC version
    subscription["method"] = "public/unsubscribe"; // Specify the method for subscription
    subscription["id"] = 1; // Unique ID for the subscription
    subscription["params"]["channels"].append("book." + symbol + ".100ms");
    
    Json::FastWriter writer; // Create a writer to convert JSON to a string
    std::string message = writer.write(subscription); // Write the JSON message to a string
    derbit_ws_.async_write(net::buffer(message), beast::bind_front_handler(&WebSocket::derbitOnWrite, shared_from_this()));
}


void WebSocket::derbitOnWrite(beast::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred); // Ignore the number of bytes transferred

    if (ec) // Check for errors
        return derbitHandleError(ec, "write"); // Handle error

    derbitReadMessages(); // Start reading messages from the WebSocket
}

void WebSocket::derbitReadMessages() {
    derbit_ws_.async_read(derbit_buffer_, beast::bind_front_handler(&WebSocket::derbitOnRead, shared_from_this()));
}

void WebSocket::derbitOnRead(beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    if (ec) 
        return fail(ec, "read");

    // Print raw received data
    std::string data = beast::buffers_to_string(buffer_.data());
    std::cout << "Raw Received Data (length " << data.length() << "): [" << data << "]" << std::endl;

    // Add more robust parsing
    try {
        Json::CharReaderBuilder builder;
        Json::CharReader* reader = builder.newCharReader();
        Json::Value root;
        std::string errors;

        bool parsingSuccessful = reader->parse(
            data.c_str(), 
            data.c_str() + data.size(), 
            &root, 
            &errors
        );

        delete reader;

        if (parsingSuccessful) {
            std::cout << "Parsed JSON:\n" << root.toStyledString() << std::endl;
            broadcastToClients(root.toStyledString());
        } else {
            std::cerr << "Failed to parse JSON. Errors: " << errors << std::endl;
            std::cerr << "Raw data: [" << data << "]" << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Exception during JSON parsing: " << e.what() << std::endl;
    }

    buffer_.consume(buffer_.size());
    derbitReadMessages();
}

// Add a method to broadcast to all connected clients
void WebSocket::broadcastToClients(const std::string& message) {
    std::lock_guard<std::mutex> lock(server_sessions_mutex_); // Lock the mutex for thread safety

    for (const auto& [client_id, session] : server_sessions_) {
        // Send the message to each connected client
        session->do_write(message); // Assuming `do_write` sends the message to the client
    }
}


void WebSocket::derbitHandleError(beast::error_code ec, char const* what) {
    std::cerr << "Error: " << what << ": " << ec.message() << std::endl;
}

// Server methods
void WebSocket::serverRunServer() {
    serverAcceptConnection();
}

void WebSocket::serverAcceptConnection() {
    server_acceptor_.async_accept(beast::bind_front_handler(&WebSocket::serverOnAccept, shared_from_this()));
}

void WebSocket::serverOnAccept(beast::error_code ec, tcp::socket socket) {
    if (!ec) {
        auto session = std::make_shared<Session>(std::move(socket), server_ssl_ctx_, this);
        std::string session_id = session->get_session_id();

        {
            std::lock_guard<std::mutex> lock(server_sessions_mutex_);
            server_sessions_[session_id] = session;
            std::cout<<"Cleint connected with client id "<<session_id<<std::endl;
        }

        session->run();
    } else {
        std::cerr << "Accept error: " << ec.message() << std::endl;
    }

    serverAcceptConnection();
}


void WebSocket::serverSubscribe(const std::string& client_id, const std::string& symbol) {
    std::lock_guard<std::mutex> lock(server_subscriptions_mutex_);
    if (server_client_subscriptions_[client_id].insert(symbol).second) { // New subscription
        if (++server_symbol_subscription_count_[symbol] == 1) { // First global subscription
            derbitSubscribeFromOrderBook(symbol); // Subscribe to Deribit
        }
    }
}


void WebSocket::serverUnsubscribe(const std::string& client_id, const std::string& symbol) {
    std::lock_guard<std::mutex> lock(server_subscriptions_mutex_);
    auto client_it = server_client_subscriptions_.find(client_id);
    if (client_it != server_client_subscriptions_.end() && client_it->second.erase(symbol)) {
        if (--server_symbol_subscription_count_[symbol] == 0) {
            server_symbol_subscription_count_.erase(symbol);
            derbitUnsubscribeFromOrderBook(symbol);
        }
    }
}




void WebSocket::serverSendMessageToClient(const std::string& client_id, const std::string& message) {
    std::lock_guard<std::mutex> lock(server_sessions_mutex_);
    auto it = server_sessions_.find(client_id);
    if (it != server_sessions_.end()) {
        it->second->do_write(message);
    } else {
        std::cerr << "Client session not found for ID: " << client_id << std::endl;
    }
}


void WebSocket::serverOnMessageReceived(const std::string& client_id, const std::string& symbol, const std::string& message) {
    if (message == "subscribe") {
        serverSubscribe(client_id, symbol);
    } else if (message == "unsubscribe") {
        serverUnsubscribe(client_id, symbol);
    } 
}

// Handles errors by printing the error message to the console.
void WebSocket::fail(beast::error_code ec, char const* what)
{
    std::cerr << what << ": " << ec.message() << "\n"; // Log the error
}


int main() {
    try {
        // Prompt the user for the instrument name
        std::string instrument;
        std::cout << "Enter the instrument name (e.g., BTC-PERPETUAL): ";
        std::getline(std::cin, instrument);

        if (instrument.empty()) {
            std::cerr << "Error: Instrument name cannot be empty.\n";
            return EXIT_FAILURE;
        }

        // Initialize separate I/O contexts and SSL contexts for client and server
        net::io_context derbit_ioc;
        ssl::context derbit_ssl_ctx(ssl::context::tlsv12_client);
        derbit_ssl_ctx.set_verify_mode(ssl::verify_peer);
        derbit_ssl_ctx.set_default_verify_paths();

        net::io_context server_ioc;
        ssl::context server_ssl_ctx(ssl::context::tlsv12_server); // Modify for server's SSL if needed
        server_ssl_ctx.set_verify_mode(ssl::verify_peer);
        server_ssl_ctx.set_default_verify_paths();

        // Define the server endpoint
        tcp::endpoint server_endpoint(tcp::v4(), 8080);

        // Initialize WebSocket for client and server functionality
        auto websocket = std::make_shared<WebSocket>(derbit_ioc,
                                                     derbit_ssl_ctx, 
                                                     server_ioc,
                                                     server_ssl_ctx,
                                                     server_endpoint
                                                     );

        // Run the Deribit client in a separate thread
        std::thread client_thread([websocket, &derbit_ioc] {
            websocket->derbitRunClient("www.deribit.com", "443", "BTC-PERPETUAL");
            derbit_ioc.run();
        });

        // Run the WebSocket server in another thread
        std::thread server_thread([websocket, &server_ioc] {
            websocket->serverRunServer();
            server_ioc.run();
        });

        // Wait for the threads to finish
        client_thread.join();
        server_thread.join();
    } 
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

