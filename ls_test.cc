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
    chdir("..");
}

TEST(ListSortedEntriesIn, IgnoreHiddenFile) {
    char filename[] = "XXXXXX";
    const char *temp_dir = mkdtemp(filename);
    chdir(temp_dir);
    std::ofstream(".aaa");
    std::ofstream("aaa");
    std::ofstream("bbb");
    auto ret2 = ListSortedEntriesIn(".", false);
    EXPECT_EQ(ret2.size(), 3);
    auto ret1 = ListSortedEntriesIn(".", true);
    EXPECT_EQ(ret1.size(), 2);
    chdir("..");
}
