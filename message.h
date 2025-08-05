#ifndef MESSAGE_H
#define MESSAGE_H
#include <cstdint>

struct GoBytes {
    const void* data;
    int64_t len;
    int64_t cap;
};

struct GoString {
    const void* data;
    int64_t len;
};

struct Message {
    int64_t pinner;
    const int32_t code;
    const int64_t object_id;
    const int32_t length;
    const u_int8_t *data;
};

#endif //MESSAGE_H
