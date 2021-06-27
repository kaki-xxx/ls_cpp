#include <cstdlib>
#include <filesystem>
#include <gtest/gtest.h>
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>
#include "ls.cc"

namespace fs = std::filesystem;

TEST(ListSortedEntriesIn, IsSorted) {
    char filename[] = "XXXXXX";
    const char *temp_dir = mkdtemp(filename);
    chdir(temp_dir);
    std::ofstream("aaa");
    std::ofstream("aba");
    std::ofstream("abb");
    auto ret = ListSortedEntriesIn(".");
    EXPECT_EQ(ret[0], fs::path("./aaa"));
    EXPECT_EQ(ret[1], fs::path("./aba"));
    EXPECT_EQ(ret[2], fs::path("./abb"));
}
