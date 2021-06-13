#include <bits/types/FILE.h>
#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

int main() {
    auto iter = fs::directory_iterator(".");
    std::vector<fs::directory_entry> files(begin(iter), end(iter));
    std::sort(std::begin(files), std::end(files));
    for (const auto& x : files) {
        std::cout << x.path().filename().generic_u8string() << std::endl;
    }
    return 0;
}
