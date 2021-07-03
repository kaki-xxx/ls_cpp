#include "ls.h"
#include "cxxopts.hpp"
#include "config.h"
#include <cstdio>
#include <cstdlib>
#include <system_error>

std::string VersionInfo() {
    char buf[16];
    std::snprintf(buf, sizeof(buf), "ls (%d.%d)", LS_VERSION_MAJOR, LS_VERSION_MINOR);
    return std::string(buf);
}

int main(int argc, char *argv[]) {
    setlocale(LC_CTYPE, "");
    cxxopts::Options options("ls", "List information about the FILEs (the current directory by default).");
    options.add_options()
        ("l", "use a long listing format")
        ("a,all", "do not ignore entries starting with .")
        ("help", "display this help and exit")
        ("version", "show version information")
    ;
    try {
        auto result = options.parse(argc, argv);
        options.custom_help("[OPTION]... [FILE]...");
        if (result.count("help")) {
            std::cout << options.help() << std::endl;
            std::exit(EXIT_SUCCESS);
        }
        if (result.count("version")) {
            std::cout << VersionInfo() << std::endl;
            std::exit(EXIT_SUCCESS);
        }
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
