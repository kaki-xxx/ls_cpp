#include "ls.h"

int main(int argc, char *argv[]) {
    Ls ls;
    ls.run(argc == 1 ? "." : argv[1]);
}
