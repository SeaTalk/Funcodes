
#include <gtest/gtest.h>
#include <iostream>
#include <string>

#include "filesys.h"

TEST(FlatTest, PathTest) {
    EXPECT_EQ(flat_path("foo/../bar"), "bar");
    EXPECT_EQ(flat_path("foo/../bar/../aaa"), "aaa");
    EXPECT_EQ(flat_path("foo/bar/..///..//aaa"), "aaa");
    EXPECT_EQ(flat_path("foo////bar//aaa"), "foo/bar/aaa");
    EXPECT_EQ(flat_path("/foo/bar/aaa"), "/foo/bar/aaa");
    EXPECT_EQ(flat_path("foo/bar/aaa"), "foo/bar/aaa");
    EXPECT_EQ(flat_path("//foo////bar///aaa"), "//foo/bar/aaa");
    EXPECT_EQ(flat_path("//foo//.////.//bar///aaa"), "//foo/bar/aaa");
    EXPECT_EQ(flat_path(".//foo//.////.//bar///aaa//.."), "foo/bar");
    EXPECT_THROW(try {
        flat_path("..//foo//..//bar///aaa");
    } catch (const path_incorrect_exception& e) {
        EXPECT_EQ(std::string(e.what()), "path incorrect");
        throw;
    }, path_incorrect_exception);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
