#ifndef LS_H
#define LS_H

#include "cxxopts.hpp"
#include <cstdio>
#include <filesystem>
#include <string>
#include <vector>

class FilesDisplayer {
public:
    virtual void DisplayFilesIn(std::filesystem::path target_path) = 0;
    virtual ~FilesDisplayer() {}
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
