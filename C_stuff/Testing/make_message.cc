#include <gtest/gtest.h>
#include <string.h>
#include "../encode_decode/encode_decode.hpp"
#include "../hash/md5.hpp"

TEST(MAKEMESSAGE, PLAYERMOVE
) {
    EXPECT_STREQ((char *) md5String("{\"x\":1,\"y\":2}"), "\xBF\xFCmu\xBA\xD8x\x1E\xAB\x9A\xAA\xD0\"\x98^k");
    EXPECT_STREQ(m_MP_PLAYER_MOVE(1, 2), "00020013{\"x\":1,\"y\":2}\xBF\xFCmu\xBA\xD8x\x1E\xAB\x9A\xAA\xD0\"\x98^k");
}
