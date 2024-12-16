#include <iostream>
#include "../include/DeribitAPI.h"
#include "../include/HFTClient.h"
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <thread>

void display_menu() {
    std::cout << "Select an action:" << std::endl;
    std::cout << "1. Authenticate" << std::endl;
    std::cout << "2. Place Order" << std::endl;
    std::cout << "3. Get Positions" << std::endl;
    std::cout << "4. Get Order Book" << std::endl;
    std::cout << "5. View Order Book Updates" << std::endl;
    std::cout << "6. Modify Order" << std::endl;
    std::cout << "7. Cancel Order" << std::endl;
    std::cout << "8. Exit" << std::endl;
}

void handle_order_book_updates(std::shared_ptr<HFTClient> client) {
    std::cout << "Do you want to (1) Subscribe or (2) Unsubscribe?" << std::endl;
    int action_choice;
    std::cin >> action_choice;

    std::string instrument;
    std::cout << "Enter the instrument name (e.g., btc-perpetual): ";
    std::cin >> instrument;

    if (action_choice == 1) {
        client->subscribe(instrument);
        std::cout << "Subscribed to order book updates for " << instrument << "." << std::endl;
    } else if (action_choice == 2) {
        client->unsubscribe(instrument);
        std::cout << "Unsubscribed from order book updates for " << instrument << "." << std::endl;
    } else {
        std::cout << "Invalid choice! Please try again." << std::endl;
    }
}

int main() {
    try {
        // Create separate I/O contexts for Deribit client and WebSocket
        boost::asio::io_context websocket_ioc;

        // Create SSL contexts
        boost::asio::ssl::context websocket_ssl_ctx(boost::asio::ssl::context::tlsv12_client);

        // Create an instance of the HFTClient as a shared pointer
        auto hft_client = std::make_shared<HFTClient>(websocket_ioc, websocket_ssl_ctx);

        // Create an instance of the DeribitAPI
        DeribitAPI api;

        // Run the WebSocket client in a separate thread
        std::thread websocket_thread([hft_client, &websocket_ioc]() {
            try {
                // Run the I/O context to process asynchronous operations
                websocket_ioc.run();
            }
            catch (const std::exception& e) {
                std::cerr << "WebSocket thread error: " << e.what() << std::endl;
            }
        });

        // Main menu loop
        while (true) {
            display_menu();
            int choice;
            std::cin >> choice;

            switch (choice) {
                case 1:
                    api.get_token();
                    break;
                case 2:
                    api.place_order();
                    break;
                case 3:
                    api.get_positions();
                    break;
                case 4:
                    api.get_order_book();
                    break;
                case 5:
                    handle_order_book_updates(hft_client);
                    break;
                case 6:
                    api.modify_order();
                    break;
                case 7:
                    api.cancel_order();
                    break;
                case 8:
                    // Cleanup and exit
                    websocket_ioc.stop();
                    websocket_thread.join();
                    return 0;
                default:
                    std::cout << "Invalid choice! Please try again." << std::endl;
            }
        }

        // Ensure the WebSocket thread is joined
        websocket_thread.join();
    }
    catch (const std::exception& e) {
        std::cerr << "Main function error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}