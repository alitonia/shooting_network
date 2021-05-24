#include <gtest/gtest.h>
#include "../hash/md5.hpp"

// Demonstrate some basic assertions.
TEST(MD5_HASH, BasicAssertions
) {
    char *s = (char *) md5String((char *) "abc");
    EXPECT_STREQ(s, "\x90\x1P\x98<\xD2O\xB0\xD6\x96?}(\xE1\x7Fr");
    free(s);
}
