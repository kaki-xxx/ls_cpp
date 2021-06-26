#include <bits/types/FILE.h>
#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <unistd.h>
#include <sys/ioctl.h>

namespace fs = std::filesystem;

int main() {
    auto iter = fs::directory_iterator(".");
    std::vector files(begin(iter), end(iter));
    std::sort(std::begin(files), std::end(files));
    for (const auto& x : files) {
        std::cout << x.path().filename().generic_u8string() << std::endl;
    }

    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1) {
        perror("ioctl");
    }

    std::cout << "width: " <<  ws.ws_col << std::endl;
    std::cout << "height: " <<  ws.ws_row << std::endl;
    return 0;
}
