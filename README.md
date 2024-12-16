# Order Execution and Management System
This project provides a C++ implementation of the Deribit API, allowing you to interact with the Deribit cryptocurrency exchange programmatically.

## Prerequisites

- CMake (version 3.10 or higher)
- cURL
- jsoncpp
- OpenSSL
- Boost (system, thread)

## Build Instructions

1. Clone the repository:
    `git clone https://github.com/Prajjawal12/Order-Execution-and-Management-System.git`

2. Navigate to the project directory:

`cd Order-Execution-and-Management-System`

3. Create a build directory:
`
mkdir build
cd build
`

4. Run CMake to generate the build files:
`cmake ..`

5. Build the project:
`make`

This will generate two executables:
- `DeribitAPIClient`: The main Deribit API client application
- `WebSocketClientExecutable`: A separate executable for the WebSocket client

## Execution

To execute the executable simply follow command below 

`./[executable_generated_name]`

## Dependencies

- [CURL](https://curl.se/): Used for making HTTP requests to the Deribit API.
- [jsoncpp](https://github.com/open-source-parsers/jsoncpp): Used for parsing and manipulating JSON data.
- [OpenSSL](https://www.openssl.org/): Used for secure communication with the Deribit API.
- [Boost](https://www.boost.org/): Used for system and threading functionality.



## License

This project is licensed under the [MIT License](LICENSE).
