#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <unistd.h>
#include <filesystem>
#include <algorithm>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <utility>
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

struct FileInfo {
    std::string filetype_permisson;
    std::size_t hard_link_count;
    std::string ownername;
    std::string groupname;
    std::size_t bytes;
    std::string access_time;
    std::string filename;
};

std::string FormatFiletypeAndPermission(mode_t mode) {
    std::string ret;
    if (S_ISDIR(mode)) {
        ret += "d";
    } else if (S_ISLNK(mode)) {
        ret += "l";
    } else {
        ret += "-";
    }
    for (int i = 2; i >= 0; --i) {
        ret += (mode >> (3*i+2)) & 1 ? "r" : "-";
        ret += (mode >> (3*i+1)) & 1 ? "w" : "-";
        ret += (mode >> (3*i))   & 1 ? "x" : "-";
    }
    return std::move(ret);
}

FileInfo GetFileInfo(fs::path target) {
    struct stat status;
    if (lstat(target.c_str(), &status) < 0) {
        throw std::system_error(errno, std::generic_category(), "Cannot execute stat");
    }
    FileInfo file_info;
    file_info.filetype_permisson = FormatFiletypeAndPermission(status.st_mode);
    file_info.hard_link_count = status.st_nlink;
    struct passwd* user_info = getpwuid(status.st_uid);
    file_info.ownername = user_info->pw_name;
    struct group* group_info = getgrgid(status.st_gid);
    file_info.groupname = group_info->gr_name;
    file_info.bytes = status.st_size;
    file_info.access_time = std::ctime(&status.st_atim.tv_sec);
    file_info.access_time.pop_back(); /* 改行を除く */
    file_info.filename = target.filename().u8string();
    return std::move(file_info);
}

class FileInfosDisplayerInLongList : public FileInfosDisplayer {
public:
    FileInfosDisplayerInLongList(DisplayFlags display_flags)
        : m_terminal_size(GetTerminalSize()),
          m_display_flags(display_flags) {}
    ~FileInfosDisplayerInLongList() = default;
    void DisplayFileInfosIn(fs::path target_path) {
        auto filepaths = ListSortedEntriesIn(target_path, m_display_flags.ignore_hidden_file);
        std::vector<FileInfo> file_infos;
        file_infos.reserve(filepaths.size());
        struct DisplayLen {
            size_t hard_link_count;
            size_t filetype_permisson;
            size_t ownername;
            size_t groupname;
            size_t bytes;
            size_t access_time;
            size_t filename;
        } display_len{};
        display_len.filetype_permisson = 10;
        display_len.access_time = 24;
        for (const auto& filepath : filepaths) {
            auto file_info = GetFileInfo(filepath);
            display_len.hard_link_count = std::max(
                display_len.hard_link_count, std::to_string(file_info.hard_link_count).length()
            );
            display_len.ownername = std::max(display_len.ownername, file_info.ownername.length());
            display_len.groupname = std::max(display_len.groupname, file_info.groupname.length());
            display_len.bytes = std::max(display_len.bytes, std::to_string(file_info.bytes).length());
            display_len.filename = std::max(display_len.filename, file_info.filename.length());
            file_infos.push_back(file_info);
        }

        const char *fmt = "%*s %*zd %*s %*s %*zd %*s %*s\n";

        for (auto file_info : file_infos) {
            std::printf(fmt,
                display_len.filetype_permisson,
                file_info.filetype_permisson.c_str(),
                display_len.hard_link_count,
                file_info.hard_link_count,
                display_len.ownername,
                file_info.ownername.c_str(),
                display_len.groupname,
                file_info.groupname.c_str(),
                display_len.bytes,
                file_info.bytes,
                display_len.access_time,
                file_info.access_time.c_str(),
                -display_len.filename,
                file_info.filename.c_str()
            );
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
        file_displayer = std::unique_ptr<FileInfosDisplayer>(
            new FileInfosDisplayerInLongList(display_flags)
        );
    } else {
        file_displayer = std::unique_ptr<FileInfosDisplayer>(
            new FileInfosDisplayerInColumns(display_flags)
        );
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
