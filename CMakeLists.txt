cmake_minimum_required(VERSION 3.10)
project(DeribitAPIClient)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(CURL REQUIRED)
find_package(jsoncpp REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS system thread)

include_directories(${CURL_INCLUDE_DIRS} ${JSONCPP_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})

# Executable for the Deribit API client
add_executable(DeribitAPIClient
    src/DeribitAPI.cpp
    src/main.cpp
    src/PlaceOrder.cpp
    src/GetPositions.cpp
    src/GetOrderBook.cpp
    src/ModifyOrder.cpp
    src/CancelOrder.cpp
    src/APIClient.cpp
    src/HFTClient.cpp
)

target_link_libraries(DeribitAPIClient PRIVATE ${CURL_LIBRARIES} jsoncpp ${OPENSSL_LIBRARIES} Boost::system Boost::thread)



# Executable for the WebSocketClient
add_executable(WebSocketClientExecutable
    src/WebSocket.cpp
    src/ClientSession.cpp
)

target_link_libraries(WebSocketClientExecutable PRIVATE ${CURL_LIBRARIES} jsoncpp ${OPENSSL_LIBRARIES} Boost::system Boost::thread)
