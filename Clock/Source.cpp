#include <iostream>
#include <chrono>
#include <thread>
#include <iomanip>
#include <ctime>

int main() {
    while (true) {
        // Get current time
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);

        // Clear the console (works on most terminals)
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif

        // Print formatted date and time
        std::tm* local_time = std::localtime(&now_c);
        std::cout << std::put_time(local_time, "%Y-%m-%d %H:%M:%S") << std::endl;

        // Wait for 1 second
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}