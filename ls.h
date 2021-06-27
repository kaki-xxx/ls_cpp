#ifndef LS_H
#define LS_H

#include "cxxopts.hpp"
#include <cstdio>
#include <filesystem>
#include <string>
#include <vector>

class FileInfosDisplayer {
public:
    virtual void DisplayFileInfosIn(std::filesystem::path target_path) = 0;
    virtual ~FileInfosDisplayer() {}
};

class Ls {
public:
    Ls(std::vector<std::string> args, cxxopts::ParseResult opts);
    ~Ls() = default;
    void Run();
private:
    std::vector<std::string> target_paths;
    std::unique_ptr<FileInfosDisplayer> m_file_displayer;
};

#endif /* LS_H */
