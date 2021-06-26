#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <filesystem>
#include <algorithm>
#include <sys/ioctl.h>
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
    std::vector files(begin(iter), end(iter));
    std::sort(std::begin(files), std::end(files));
    for (const auto& x : files) {
        std::cout << x.path().filename().generic_u8string() << std::endl;
    }
}
