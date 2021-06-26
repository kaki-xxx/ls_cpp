#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <unistd.h>
#include <filesystem>
#include <algorithm>
#include <sys/ioctl.h>
#include <vector>
#include "ls.h"

namespace fs = std::filesystem;

TerminalSize GetTerminalSize() {
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1) {
        throw std::system_error(errno, std::generic_category(), "Cannot get terminal size information");
    }

    TerminalSize ret;
    ret.row = ws.ws_row;
    ret.col = ws.ws_col;
    return std::move(ret);
}

Ls::Ls() : terminal_size(GetTerminalSize()) {}

void Ls::run() {
    auto iter = fs::directory_iterator(".");
    std::vector filepaths(begin(iter), end(iter));
    std::sort(std::begin(filepaths), std::end(filepaths));
    size_t display_len = 0;
    std::vector<std::string> files;
    files.reserve(filepaths.size());
    for (const auto& file : filepaths) {
        std::string filename = file.path().filename().generic_u8string();
        files.push_back(filename);
        display_len = std::max(display_len, filename.length() + 2);
    }
    size_t number_per_onerow = terminal_size.col / display_len;
    size_t number_of_rows = (filepaths.size() + number_per_onerow-1) / number_per_onerow;
    std::ios::fmtflags prev_flags = std::cout.setf(std::ios::left, std::ios::adjustfield);
    for (size_t row = 0; row < number_of_rows; row++) {
        for (size_t col = row; col < filepaths.size(); col += number_of_rows) {
            std::cout << std::setw(display_len) << files[col];
        }
        std::cout << std::endl;
    }
    std::cout.flags(prev_flags);
}
