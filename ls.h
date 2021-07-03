#ifndef LS_H
#define LS_H

#include "cxxopts.hpp"
#include <cstdio>
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

class FilesLister {
public:
    virtual void ListFiles(fs::path target_path) = 0;
    virtual ~FilesLister() {}
};

class Ls {
public:
    Ls(std::vector<std::string> args, cxxopts::ParseResult opts);
    ~Ls() = default;
    void Run();
private:
    std::vector<std::string> target_paths;
    std::unique_ptr<FilesLister> m_file_lister;
};

#endif /* LS_H */
