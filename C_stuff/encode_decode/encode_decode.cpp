#ifndef NETWORK_CLIENT_ENCODE_DECODE_C
#define NETWORK_CLIENT_ENCODE_DECODE_C

#include "math.h"
#include <nlohmann/json.hpp>
#include "encode_decode.hpp"
#include "../hash/md5.hpp"

#define BYTES_FOR_MESSAGE_TYPE 4
#define BYTES_FOR_MESSAGE_LENGTH 4
#define BYTES_FOR_MESSAGE_HASH 16
#define MAX_MSG_LENGTH BYTES_FOR_MESSAGE_TYPE + BYTES_FOR_MESSAGE_LENGTH + BYTES_FOR_MESSAGE_HASH + pow(10, BYTES_FOR_MESSAGE_LENGTH)

using json = nlohmann::json;

bool validate_C_message(int code, int length, char *payload, uint8_t *hash) {
    if (code < MC_SEND || code > C_TEST) {
        return false;
    }
    if (length < 0) {
        return false;
    }
    if ((payload && strlen(payload) != length) || (!payload && length != 0)) {
        return false;
    }
    if (length > 0 && (
            !hash
            || (strlen(((char *) hash)) != BYTES_FOR_MESSAGE_HASH)
            || (strcmp((char *) md5String(payload), (char *) hash) != 0)
    )) {
        return false;
    }
    return true;
}

C_message *create_message_struct(enum message_type t, char *payload) {
    C_message *m = (C_message *) malloc(sizeof(C_message));
    int length;
    if (payload == nullptr) {
        length = 0;
        m->payload = nullptr;
        m->hash = (uint8_t *) malloc(BYTES_FOR_MESSAGE_HASH + 1);
        memset(m->hash, 0, BYTES_FOR_MESSAGE_HASH + 1);
    } else {
        length = strlen(payload);
        m->payload = payload;
        m->hash = md5String(payload);
    }
    m->t = t;
    m->length = length;
    return m;
}

bool destroy_message_struct(C_message *m) {
    if (m) {
        free(m->hash);
        free(m);
    }
    return true;
}


char *ms_to_char(const C_message *m) {
    if (!m) {
        return nullptr;
    }

    const int message_length = (
            m->length
            + BYTES_FOR_MESSAGE_TYPE
            + BYTES_FOR_MESSAGE_LENGTH
            + BYTES_FOR_MESSAGE_HASH
    );

    char *rm = (char *) malloc(sizeof(char *) * message_length + 1);
    if (m->length > 0) {
        sprintf(rm, "%04d%04d%s%s", m->t, m->length, m->payload, m->hash);
    } else {
        sprintf(rm, "%04d%04d%s", m->t, m->length, m->hash);
    }
    rm[message_length] = '\0';
    return rm;
}


C_message *char_to_ms(char *s, int msg_len, bool *flag) {
    int message_code = 0;
    int payload_length = 0;
    char *payload = nullptr;
    char *message_hash = (char *) malloc(sizeof(char) * BYTES_FOR_MESSAGE_HASH);
    memset(message_hash, 0, sizeof(char) * BYTES_FOR_MESSAGE_HASH);

    if (!s || msg_len == 0) {
        return nullptr;
    }

    for (int i = 0; i < msg_len; ++i) {
        if (i < BYTES_FOR_MESSAGE_TYPE) {
            message_code = message_code * 10 + (s[i] - '0');
        } else if (i < (BYTES_FOR_MESSAGE_TYPE + BYTES_FOR_MESSAGE_LENGTH)) {
            payload_length += payload_length * 10 + (s[i] - '0');
        } else if (i < (BYTES_FOR_MESSAGE_TYPE + BYTES_FOR_MESSAGE_LENGTH + payload_length) && payload_length > 0) {
            if (!payload) {
                payload = (char *) malloc(sizeof(char) * payload_length);
            }
            payload[i - (BYTES_FOR_MESSAGE_TYPE + BYTES_FOR_MESSAGE_LENGTH)] = s[i];
        } else {
            int offset = i - (BYTES_FOR_MESSAGE_TYPE + BYTES_FOR_MESSAGE_LENGTH + payload_length);
            if (offset < 32) {
                message_hash[offset] = s[i];
            }
        }
    }
    *flag = validate_C_message(message_code, payload_length, payload, (uint8_t *) message_hash);
    return create_message_struct((message_type) message_code, payload);
}


char *make_message(enum message_type t, char *payload) {
    auto cs = create_message_struct(t, payload);
    auto s = ms_to_char(cs);
    destroy_message_struct(cs);
    return s;
}

char *marshmallow(json j) {
    int l = j.dump().length();
    char *m = (char *) malloc(sizeof(char) * l + 1);
    strcpy(m, j.dump().c_str());
    m[l] = '\0';
    return m;
}


char *m_MP_PLAYER_MOVE(int x, int y) {
    json j = {
            {"x", x},
            {"y", y}
    };
    char *m = marshmallow(j);
    auto f = make_message(MP_PLAYER_MOVE, m);
    free(m);
    return f;
}

char *m_MP_PLAYER_SHOOT(int x, int y) {
    json j = {
            {"x", x},
            {"y", y}
    };
    char *m = marshmallow(j);
    char *f = make_message(MP_PLAYER_SHOOT, m);
    free(m);
    return f;
}

char *m_MP_MAKE_PLAYER(char *id, char *name) {
    json j = {
            {"id", id},
            {"y",  name}
    };
    char *m = marshmallow(j);
    auto f = make_message(MP_MAKE_PLAYER, m);
    free(m);
    return f;
}

#endif //NETWORK_CLIENT_ENCODE_DECODE_C
