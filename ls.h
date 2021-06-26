#ifndef LS_H
#define LS_H

#include <filesystem>

struct TerminalSize {
    unsigned short row;
    unsigned short col;
};

TerminalSize GetTerminalSize();

class Ls {
public:
    Ls();
    ~Ls() = default;
    void run(std::filesystem::path target_path);
private:
    TerminalSize terminal_size;
};

#endif /* LS_H */
