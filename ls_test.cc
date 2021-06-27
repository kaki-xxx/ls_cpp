#include <cstdlib>
#include <filesystem>
#include <gtest/gtest.h>
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>
#include "ls.cc"

namespace fs = std::filesystem;

std::string MkTempDirAndCreateFiles(std::vector<std::string> files) {
    char filename[] = "XXXXXX";
    const char *temp_dir = mkdtemp(filename);
    for (auto file : files) {
        std::ofstream(fs::path(temp_dir) / file);
    }
    return std::string(filename);
}

TEST(ListSortedEntriesIn, IsSorted) {
    auto temp_dir = MkTempDirAndCreateFiles({"aaa", "aba", "abb"});
    auto ret = ListSortedEntriesIn(temp_dir);
    EXPECT_EQ(ret[0], fs::path(temp_dir) / "aaa");
    EXPECT_EQ(ret[1], fs::path(temp_dir) / "aba");
    EXPECT_EQ(ret[2], fs::path(temp_dir) / "abb");
}

TEST(ListSortedEntriesIn, IgnoreHiddenFile) {
    auto temp_dir = MkTempDirAndCreateFiles({".aaa", "aba", "abb"});
    auto ret1 = ListSortedEntriesIn(temp_dir, false);
    EXPECT_EQ(ret1.size(), 3);
    auto ret2 = ListSortedEntriesIn(temp_dir, true);
    EXPECT_EQ(ret2.size(), 2);
}

TEST(GetFileInfo, FiletypeAndPermisson) {
    auto temp_dir = MkTempDirAndCreateFiles({"test"});
    auto file_info1 = GetFileInfo(fs::path(temp_dir) / "test");
    EXPECT_EQ(file_info1.filetype_permisson, "-rw-r--r--");
}
