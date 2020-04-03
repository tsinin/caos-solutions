#include <stdio.h>
#include <stdint.h>

typedef enum {
    PlusZero      = 0x00,
    MinusZero     = 0x01,
    PlusInf       = 0xF0,
    MinusInf      = 0xF1,
    PlusRegular   = 0x10,
    MinusRegular  = 0x11,
    PlusDenormal  = 0x20,
    MinusDenormal = 0x21,
    SignalingNaN  = 0x30,
    QuietNaN      = 0x31
} float_class_t;

extern float_class_t
classify(double *value_ptr) {
    typedef union {
        double     real_value;
        uint64_t   uint_value;
    } real_or_uint;
    real_or_uint value;
    value.real_value = *value_ptr;
    if (value.uint_value >> 63) {
        if (!(value.uint_value << 1)) {
            return MinusZero;
        }
        if (!(((value.uint_value << 1) >> 53) ^ ((1ull << 11) - 1))) {
            if (!(value.uint_value << 12)) {
                return MinusInf;
            }
            if ((value.uint_value << 12) >> 63) {
                return QuietNaN;
            }
            return SignalingNaN;
        }
        if (!((value.uint_value << 1) >> 53)) {
            return MinusDenormal;
        }
        return MinusRegular;
    } else {
        if (!(value.uint_value << 1)) {
            return PlusZero;
        }
        if (!(((value.uint_value << 1) >> 53) ^ ((1ull << 11) - 1))) {
            if (!(value.uint_value << 12)) {
                return PlusInf;
            }
            if ((value.uint_value << 12) >> 63) {
                return QuietNaN;
            }
            return SignalingNaN;
        }
        if (!((value.uint_value << 1) >> 53)) {
            return PlusDenormal;
        }
        return PlusRegular;
    }
}
