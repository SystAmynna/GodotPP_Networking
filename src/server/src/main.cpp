#include <atomic>
#include <iostream>
#include <snl.h>
#include <csignal>
#include <vector>
#include <algorithm>
#include <cstring>
#include <memory>

const char* bind_address = "localhost:3666";
std::vector<std::string> addresses;

GameSocket * socket_ptr = nullptr;
std::atomic<bool> should_running{true};

void signal_handler(const int signum) {
    if (signum == SIGINT || signum == SIGTERM) {
        should_running = false;
    }
}

void init() {
    // Register signal handler for graceful shutdown
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    // Create and bind the socket
    socket_ptr = net_socket_create(bind_address);
    if (!socket_ptr) {
        std::cerr << "Failed to create socket on " << bind_address << std::endl;
        exit(EXIT_FAILURE);
    }
}

void cleanup() {
    if (socket_ptr) {
        net_socket_destroy(socket_ptr);
        socket_ptr = nullptr;
    }
}

void run() {
    constexpr unsigned int data_size = 1024;
    constexpr unsigned int sender_size = 256;

    // Initialise needed data
    const auto data = std::make_unique<uint8_t[]>(data_size);
    const auto out_sender = std::make_unique<char[]>(sender_size);

    // Main loop to poll for incoming data
    while (should_running) {

        // Clear the data buffer and sender information before polling
        std::memset(data.get(), 0, data_size);
        std::memset(out_sender.get(), 0, sender_size);

        // Poll the socket for incoming data
        const int32_t result = net_socket_poll(socket_ptr, data.get(), data_size, out_sender.get(), sender_size);
        // Check the result
        if (result < 0) {
            std::cerr << "Failed to poll socket on " << bind_address << std::endl;
            break;
        }
        if (result == 0) continue;
        std::cout << "Received " << result << " bytes from " << out_sender.get() << std::endl << std::flush;

        // Add the sender to the list of known addresses if it's not already there
        if (out_sender[0] != '\0' && std::find(addresses.begin(), addresses.end(), out_sender.get()) == addresses.end()) {
            addresses.emplace_back(out_sender.get());
            std::cout << "New address added: " << out_sender.get() << std::endl << std::flush;
        }

        // Broadcast the received data to all other connected clients
        for (const std::string& address : addresses) {
            if (address == out_sender.get()) continue;

            const int32_t send_result = net_socket_send(socket_ptr, address.c_str(), data.get(), static_cast<uintptr_t>(result));
            if (send_result < 0) {
                std::cerr << "Failed to send data to " << address << std::endl;
            }
        }

    }
}

int main() {
    // init
    std::cout << "Starting the server..." << std::endl;
    init();

    // run
    std::cout << "Running the server..." << std::endl;
    run();

    // cleanup
    std::cout << "Cleaning up resources..." << std::endl;
    cleanup();

    return 0;
}