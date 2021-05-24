#include <gtest/gtest.h>
#include <string.h>
#include "../hash/md5.hpp"
#include "../encode_decode/encode_decode.hpp"

TEST(VALID_FORMAT, VALIDMESSAGECREATION
) {
    EXPECT_EQ(validate_C_message(0, 0, nullptr, nullptr), true);
    auto s = (char *) "a";
    EXPECT_EQ(validate_C_message(0, 1, s, md5String(s)), true);
    s = (char *) "adfe";
    EXPECT_EQ(validate_C_message(0, 4, s, md5String(s)), true);
    EXPECT_NE(validate_C_message(-1, 1, s, md5String(s)), true);
    EXPECT_NE(validate_C_message(0, 3, s, md5String(s)), true);
    EXPECT_NE(validate_C_message(0, 4, s, nullptr), true);
    EXPECT_NE(validate_C_message(-1, 1, s, (uint8_t *) "abc"), true);
}

struct ENCODE : public testing::Test {
protected:
    C_message *ms;
    bool f;

    void SetUp() {
        ms = create_message_struct(MC_SEND, (char *) "abc");
    }

    void TearDown() {
        destroy_message_struct(ms);
    }
};

TEST_F(ENCODE, VALIDMESSAGECREATION
) {
    EXPECT_STREQ(ms->payload, "abc");
    EXPECT_STREQ((char *) ms->hash, "\x90\x1P\x98<\xD2O\xB0\xD6\x96?}(\xE1\x7Fr");
    EXPECT_EQ(ms->t, MC_SEND);
    EXPECT_EQ(ms->length, 3);
}

TEST_F(ENCODE, STRUCTTOCHAR
) {
    auto m = ms_to_char(ms);

    EXPECT_STREQ(m, "00000003abc\x90\x1P\x98<\xD2O\xB0\xD6\x96?}(\xE1\x7Fr");
}

TEST_F(ENCODE, CHARTOSTRUCT
) {
    auto m = ms_to_char(ms);
    auto ms1 = char_to_ms(m, strlen(m), &f);
    EXPECT_EQ(f, true);
    EXPECT_EQ(ms1->t, 0);
    EXPECT_EQ(ms1->length, 3);
    EXPECT_STREQ(ms1->payload, "abc");
    EXPECT_STREQ((char *) ms1->hash, "\x90\x1P\x98<\xD2O\xB0\xD6\x96?}(\xE1\x7Fr");
}


struct ENCODENULL : public testing::Test {
protected:
    C_message *ms;
    bool f;


    void SetUp() {
        ms = create_message_struct(MC_SEND, nullptr);
    }

    void TearDown() {
        destroy_message_struct(ms);
    }
};


TEST_F(ENCODENULL, VALIDMESSAGECREATION
) {
    EXPECT_EQ(ms->t, MC_SEND);
    EXPECT_EQ(ms->length, 0);
}

TEST_F(ENCODENULL, STRUCTTOCHAR
) {
    auto m = ms_to_char(ms);

    EXPECT_STREQ(m, "00000000");
}

TEST_F(ENCODENULL, CHARTOSTRUCT
) {
    auto m = ms_to_char(ms);
    auto ms1 = char_to_ms(m, strlen(m), &f);
    EXPECT_EQ(f, true);
    EXPECT_EQ(ms1->t, MC_SEND);
    EXPECT_EQ(ms1->length, 0);
    EXPECT_STREQ(ms1->payload, nullptr);
    EXPECT_STREQ((char *) ms1->hash, "");
}