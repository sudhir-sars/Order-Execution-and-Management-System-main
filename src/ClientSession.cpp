#include "../include/ClientSession.h"
#include <iostream>
#include <jsoncpp/json/json.h>
Session::Session(tcp::socket socket, ssl::context& ctx, WebSocket* server)
    : ws_(std::move(socket), ctx), server_(server), session_id_(generate_session_id()) {}


void Session::run() {

    ws_.next_layer().async_handshake(ssl::stream_base::server,
                beast::bind_front_handler(&Session::on_handshake, shared_from_this()));
}

std::string Session::get_session_id() const {
    return session_id_;
}

std::string Session::generate_session_id() {
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    return to_string(uuid);
}

void Session::on_handshake(beast::error_code ec) {
    if (!ec) {
        ws_.async_accept(beast::bind_front_handler(&Session::on_accept, shared_from_this()));
    } else {
        std::cerr << "SSL handshake error: " << ec.message() << std::endl;
    }
}

void Session::on_accept(beast::error_code ec) {
    if (ec) {
        std::cerr << "WebSocket accept error: " << ec.message() << std::endl;
        return;
    }
    do_read();
}

void Session::do_read() {
    ws_.async_read(buffer_, beast::bind_front_handler(&Session::on_read, shared_from_this()));
}

void Session::on_read(beast::error_code ec, std::size_t bytes_transferred) {
    if (ec) {
        std::cerr << "WebSocket read error: " << ec.message() << std::endl;
        return;
    }

    auto message = beast::buffers_to_string(buffer_.data());
    buffer_.consume(buffer_.size());  // Clear the buffer

    try {
        // Parse the JSON message
        Json::CharReaderBuilder readerBuilder;
        Json::Value json;
        std::string errors;

        std::istringstream stream(message);
        if (!Json::parseFromStream(readerBuilder, stream, &json, &errors)) {
            throw std::runtime_error("Failed to parse JSON: " + errors);
        }

        std::string action = json["action"].asString();
        std::string symbol = json["symbol"].asString();

        if (action == "subscribe") {
            server_->serverSubscribe(session_id_, symbol);  // Notify server about subscription
        } else if (action == "unsubscribe") {
            server_->serverUnsubscribe(session_id_, symbol);  // Notify server about unsubscription
        }
    } catch (const std::exception& e) {
        std::cerr << "Error parsing message: " << e.what() << std::endl;
    }

    do_read();  // Continue reading
}

void Session::do_write(const std::string& message) {
    // Create a JSON value to structure the message
    Json::Value jsonMessage;
    jsonMessage["message"] = message;
    
    // Use StreamWriterBuilder to convert JSON to string
    Json::StreamWriterBuilder writer;
    std::string serializedMessage = Json::writeString(writer, jsonMessage);

    // Perform async write to the WebSocket
    ws_.async_write(
        net::buffer(serializedMessage), 
        beast::bind_front_handler(&Session::on_write, shared_from_this())
    );
}

void Session::on_write(beast::error_code ec, std::size_t bytes_transferred) {
    if (ec) {
        std::cerr << "Error sending message: " << ec.message() << std::endl;
        return;
    }
    
    // Optional: Log successful message send
    std::cout << "Message sent successfully. Bytes transferred: " << bytes_transferred << std::endl;
}
