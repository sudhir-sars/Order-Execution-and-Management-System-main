#ifndef SESSION_H
#define SESSION_H

#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <memory>
#include <string>
#include <vector>
#include "WebSocket.h"




class WebSocket;  // Forward declaration


namespace beast = boost::beast;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;


class Session : public std::enable_shared_from_this<Session> {
public:

    Session(tcp::socket socket, ssl::context& ctx, WebSocket* server);

    void run();
    std::string get_session_id() const;

    void do_write(const std::string& message);
    void on_write(beast::error_code ec, std::size_t bytes_transferred);
    
    // Add your message handling functions as needed...

private:
    std::string generate_session_id();
    void on_handshake(beast::error_code ec);
    void on_accept(beast::error_code ec);
    void do_read();
    void on_read(beast::error_code ec, std::size_t bytes_transferred);


    
    // WebSocket object to manage the connection
    beast::websocket::stream<beast::ssl_stream<tcp::socket>> ws_;
    beast::flat_buffer buffer_;  // Buffer for incoming messages
    WebSocket* server_;
    std::string session_id_; // Store the session ID
};

#endif
