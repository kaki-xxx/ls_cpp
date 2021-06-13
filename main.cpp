#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

int main() {
    for (const auto& x : fs::directory_iterator(".")) {
        std::cout << x.path().filename().generic_u8string() << std::endl;
    }
    return 0;
}
