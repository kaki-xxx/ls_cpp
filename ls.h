#ifndef LS_H
#define LS_H

struct TerminalSize {
    unsigned short row;
    unsigned short col;
};

TerminalSize GetTerminalSize();

class Ls {
public:
    Ls();
    ~Ls() = default;
    void run();
private:
    TerminalSize terminal_size;
};

#endif /* LS_H */
