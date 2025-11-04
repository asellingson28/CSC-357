#include <iostream>
#include <csignal>
#include <unistd.h>

void signalHandler(int signum) {
    std::cout << "\nInterrupt signal (" << signum << ") received.\n";
    // cleanup or save state here
    exit(signum);
}

int main() {
    // Register signal handler for SIGINT (Ctrl+C)
    signal(SIGINT, signalHandler);

    while (true) {
        std::cout << "Running... Press Ctrl+C to stop.\n";
        sleep(1);
    }
    return 0;
}
