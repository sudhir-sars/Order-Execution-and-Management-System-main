#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include "ClientSession.h"  // Ensure this is present
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <jsoncpp/json/json.h>
#include <cstdlib>
#include <memory>
#include <mutex>
#include <map>
#include <set>
#include <string>
#include "ClientSession.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;

class Session;  // Forward declaration


/**
 * @class WebSocket
 * @brief Combines WebSocket client and server functionality into a single class for real-time communication.
 */
class WebSocket : public std::enable_shared_from_this<WebSocket> {
public:
    // Constructor for derbit client functionality
    explicit WebSocket(net::io_context& derbit_ioc,
                     ssl::context& derbit_ssl_ctx,
                     net::io_context& server_ioc,
                     ssl::context& server_ssl_ctx,
                     tcp::endpoint server_endpoint);

   
    // Client methods
    void derbitRunClient(const std::string& host, const std::string& port, const std::string& instrument);

    // Server methods
    void serverRunServer();
    void serverSubscribe(const std::string& client_id, const std::string& symbol);
    void serverUnsubscribe(const std::string& client_id, const std::string& symbol);
    

private:
    // Client methods
    void derbitOnResolve(beast::error_code ec, tcp::resolver::results_type results);
    void derbitOnConnect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep);
    void derbitOnSSLHandshake(beast::error_code ec);
    void derbitOnHandshake(beast::error_code ec);
    void derbitSubscribeFromOrderBook(const std::string& symbol);
    void derbitUnsubscribeFromOrderBook(const std::string& symbol);
    void derbitOnWrite(beast::error_code ec, std::size_t bytes_transferred);
    void derbitOnRead(beast::error_code ec, std::size_t bytes_transferred);
    
    void derbitReadMessages();
    void derbitHandleError(beast::error_code ec, char const* what);

    // Server methods
    void serverAcceptConnection();
    void serverOnAccept(beast::error_code ec, tcp::socket socket);
    void serverSendMessageToClient(const std::string& client_id, const std::string& message);
    void serverOnMessageReceived(const std::string& client_id, const std::string& symbol, const std::string& message);
    void broadcastToClients(const std::string& message);
    
    void fail(beast::error_code ec, char const* what);

    // Shared resources
    net::io_context& derbit_oc_;
    ssl::context& derbit_ssl_ctx_;
    // Shared resources
    net::io_context& server_ioc_;
    ssl::context& server_ssl_ctx_;

    // Derbit-specific members
    tcp::resolver derbit_resolver_;
    websocket::stream<beast::ssl_stream<beast::tcp_stream>> derbit_ws_;
    beast::flat_buffer derbit_buffer_;
    std::string derbit_host_;
    std::string derbit_instrument_;
    std::vector<std::string> derbit_instruments_;

    // Server-specific members
    tcp::acceptor server_acceptor_;
    std::map<std::string, std::set<std::string>> server_client_subscriptions_;
    std::map<std::string, int> server_symbol_subscription_count_;
    std::mutex server_subscriptions_mutex_;
    std::map<std::string, std::shared_ptr<Session>> server_sessions_;
    std::mutex server_sessions_mutex_;
    beast::flat_buffer buffer_; ///< Buffer for storing incoming WebSocket messages.
};

#endif // WEBSOCKET_H
