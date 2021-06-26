#include "ls.h"
#include "cxxopts.hpp"
#include <cstdlib>
#include <system_error>

int main(int argc, char *argv[]) {
    cxxopts::Options options("ls", "List information about the FILEs (the current directory by default).");
    options.add_options()
        ("l", "use a long listing format")
    ;
    try {
        auto result = options.parse(argc, argv);
        options.custom_help("[OPTION]... [FILE]...");
        auto args = result.unmatched();
        Ls ls(args, result);
        ls.Run();
    } catch(cxxopts::OptionException e) {
        std::cerr << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    } catch (std::system_error e) {
        std::cerr << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}
