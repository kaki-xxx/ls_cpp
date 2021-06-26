#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <iterator>
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

struct DisplayFlags {
    bool ignore_hidden_file;
    DisplayFlags() : ignore_hidden_file(true) {};
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

bool IsHiddenFile(fs::path target) {
    std::string filename = target.filename().u8string();
    return filename[0] == '.';
}

std::vector<std::filesystem::directory_entry>
ListSortedEntriesIn(std::filesystem::path target_path, bool ignore_hidden_file = false) {
    auto iter = fs::directory_iterator(target_path);
    std::vector filepaths(begin(iter), end(iter));
    std::sort(std::begin(filepaths), std::end(filepaths));
    std::vector<fs::directory_entry> ret;
    ret.reserve(filepaths.size());
    for (auto filepath : filepaths) {
        if (ignore_hidden_file && IsHiddenFile(filepath)) {
            continue;
        }
        ret.push_back(filepath);
    }
    ret.shrink_to_fit();
    return std::move(ret);
}

class FileInfosDisplayerInColumns : public FileInfosDisplayer {
public:
    FileInfosDisplayerInColumns(DisplayFlags display_flags)
        : m_terminal_size(GetTerminalSize()),
          m_display_flags(display_flags) {}
    ~FileInfosDisplayerInColumns() = default;

    void DisplayFileInfosIn(fs::path target_path) {
        auto filepaths = ListSortedEntriesIn(target_path, m_display_flags.ignore_hidden_file);
        size_t display_len = 0;
        std::vector<std::string> files;
        files.reserve(filepaths.size());
        for (const auto& file : filepaths) {
            std::string filename = file.path().filename().generic_u8string();
            files.push_back(filename);
            display_len = std::max(display_len, filename.length() + 2);
        }
        size_t number_per_onerow = m_terminal_size.col / display_len;
        size_t number_of_rows = (filepaths.size() + number_per_onerow-1) / number_per_onerow;
        std::ios::fmtflags prev_flags = std::cout.setf(std::ios::left, std::ios::adjustfield);
        for (size_t row = 0; row < number_of_rows; row++) {
            for (size_t col = row; col < files.size(); col += number_of_rows) {
                std::cout << std::setw(display_len) << files[col];
            }
            std::cout << '\n';
        }
        std::cout.flags(prev_flags);
    }
private:
    TerminalSize m_terminal_size;
    DisplayFlags m_display_flags;
};

class FileInfosDisplayerInLongList : public FileInfosDisplayer {
public:
    FileInfosDisplayerInLongList(DisplayFlags display_flags)
        : m_terminal_size(GetTerminalSize()),
          m_display_flags(display_flags) {}
    ~FileInfosDisplayerInLongList() = default;
    void DisplayFileInfosIn(fs::path target_path) {
        auto filepaths = ListSortedEntriesIn(target_path, m_display_flags.ignore_hidden_file);
        std::vector<std::string> files;
        files.reserve(filepaths.size());
        for (const auto& file : filepaths) {
            std::string filename = file.path().filename().generic_u8string();
            files.push_back(filename);
        }
    }
private:
    TerminalSize m_terminal_size;
    DisplayFlags m_display_flags;
};
} /* unnamed namespace */

Ls::Ls(
    std::vector<std::string> args,
    cxxopts::ParseResult opts)
        : target_paths(args) {
    DisplayFlags display_flags;
    if (opts.count("a")) {
        display_flags.ignore_hidden_file = false;
    }
    if (opts.count("l")) {
        file_displayer = std::unique_ptr<FileInfosDisplayer>(new FileInfosDisplayerInLongList(display_flags));
    } else {
        file_displayer = std::unique_ptr<FileInfosDisplayer>(new FileInfosDisplayerInColumns(display_flags));
    }
}

void Ls::Run() {
    if (target_paths.size() == 0) {
        file_displayer->DisplayFileInfosIn(".");
        return;
    }
    for (auto target_path : target_paths) {
        file_displayer->DisplayFileInfosIn(target_path);
    }
}
