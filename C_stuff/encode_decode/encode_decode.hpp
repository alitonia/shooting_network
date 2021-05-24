#ifndef NETWORK_CLIENT_ENCODE_DECODE_H
#define NETWORK_CLIENT_ENCODE_DECODE_H

#include <stdint.h>
#include "../hash/md5.hpp"

enum message_type {
    MC_SEND = 0,
    MP_MAKE_PLAYER,
    MP_PLAYER_MOVE,
    MP_PLAYER_SHOOT,
    C_TEST
};


struct C_message {
    enum message_type t; //4
    int length;     // 4
    char *payload;
    uint8_t *hash; //16
};

bool validate_C_message(int code, int length, char *payload, uint8_t *hash);

C_message *create_message_struct(enum message_type t, char *payload);

bool destroy_message_struct(C_message *m);

char *ms_to_char(const C_message *m);

C_message *char_to_ms(char *s, int msg_len, bool *flag);

char *make_message(enum message_type t, char *payload);

char *m_MP_PLAYER_MOVE(int x, int y);

#endif //NETWORK_CLIENT_ENCODE_DECODE_H
