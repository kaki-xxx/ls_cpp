#ifndef LS_H
#define LS_H

#include "cxxopts.hpp"
#include <cstdio>
#include <filesystem>
#include <string>
#include <vector>

struct TerminalSize {
    unsigned short row;
    unsigned short col;
};

TerminalSize GetTerminalSize();

class FilesDisplayer {
public:
    virtual void DisplayFilesIn(std::filesystem::path target_path) = 0;
    virtual ~FilesDisplayer() {}
};

static std::vector<std::filesystem::directory_entry>
ListSortedEntriesIn(std::filesystem::path target_path);

class FilesDisplayerInColumns : public FilesDisplayer {
public:
    FilesDisplayerInColumns();
    ~FilesDisplayerInColumns() = default;
    void DisplayFilesIn(std::filesystem::path target_path) override;
private:
    TerminalSize terminal_size;
};

class FilesDisplayerInLongList : public FilesDisplayer {
public:
    FilesDisplayerInLongList();
    ~FilesDisplayerInLongList() = default;
    void DisplayFilesIn(std::filesystem::path target_path) override;
private:
    TerminalSize terminal_size;
};

class Ls {
public:
    Ls(std::vector<std::string> args, cxxopts::ParseResult opts);
    ~Ls() = default;
    void Run();
private:
    std::vector<std::string> target_paths;
    std::unique_ptr<FilesDisplayer> file_displayer;
};

#endif /* LS_H */
