#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <unistd.h>
#include <filesystem>
#include <algorithm>
#include <sys/ioctl.h>
#include <vector>
#include "ls.h"
#include "cxxopts.hpp"

namespace fs = std::filesystem;

namespace {
struct TerminalSize {
    unsigned short row;
    unsigned short col;
};

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

std::vector<std::filesystem::directory_entry>
ListSortedEntriesIn(std::filesystem::path target_path) {
    auto iter = fs::directory_iterator(target_path);
    std::vector filepaths(begin(iter), end(iter));
    std::sort(std::begin(filepaths), std::end(filepaths));
    return std::move(filepaths);
}

class FilesDisplayerInColumns : public FilesDisplayer {
public:
    FilesDisplayerInColumns()
        : terminal_size(GetTerminalSize()) {}
    ~FilesDisplayerInColumns() = default;

    void DisplayFilesIn(fs::path target_path) {
        auto filepaths = ListSortedEntriesIn(target_path);
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
private:
    TerminalSize terminal_size;
};

class FilesDisplayerInLongList : public FilesDisplayer {
public:
    FilesDisplayerInLongList()
        : terminal_size(GetTerminalSize()) {}
    ~FilesDisplayerInLongList() = default;
    void DisplayFilesIn(fs::path target_path) {}
private:
    TerminalSize terminal_size;
};
} /* unnamed namespace */

Ls::Ls(
    std::vector<std::string> args,
    cxxopts::ParseResult opts)
        : target_paths(args) {
    if (opts.count("l")) {
        file_displayer = std::unique_ptr<FilesDisplayer>(new FilesDisplayerInLongList());
    } else {
        file_displayer = std::unique_ptr<FilesDisplayer>(new FilesDisplayerInColumns());
    }
}

void Ls::Run() {
    if (target_paths.size() == 0) {
        file_displayer->DisplayFilesIn(".");
        return;
    }
    for (auto target_path : target_paths) {
        file_displayer->DisplayFilesIn(target_path);
    }
}
