#pragma once

#include <Mary/UTF.h>

#undef MARY_UTF_32_To_Units_8
#undef MARY_UTF_32_To_Units_16
#undef MARY_UTF_Ptr_8_To_Units_8
#undef MARY_UTF_Ptr_16_To_Units_16
#undef MARY_UTF_Encode_8
#undef MARY_UTF_Decode_8
#undef MARY_UTF_Decode_8_Reverse
#undef MARY_UTF_Encode_16
#undef MARY_UTF_Decode_16
#undef MARY_UTF_Decode_16_Reverse
#undef MARY_UTF_Encode_32
#undef MARY_UTF_Decode_32
#undef MARY_UTF_Decode_32_Reverse

////// Sizing //////

#define MARY_UTF_32_To_Units_8(UTF_32)  \
(                                       \
    ((UTF_32) < 0x000080) ? (           \
        1                               \
    ) : ((UTF_32) < 0x000800) ? (       \
        2                               \
    ) : ((UTF_32) < 0x010000) ? (       \
        3                               \
    ) : ((UTF_32) < 0x110000) ? (       \
        4                               \
    ) : (                               \
        0                               \
    )                                   \
)

#define MARY_UTF_32_To_Units_16(UTF_32) \
(                                       \
    ((UTF_32) < 0x10000) ? (            \
        1                               \
    ) : (                               \
        2                               \
    )                                   \
)

#define MARY_UTF_Ptr_8_To_Units_8(PTR)  \
(                                       \
    (*PTR >> 7 == 0x00) ? (             \
        1                               \
    ) : (*PTR >> 5 == 0x06) ? (         \
        2                               \
    ) : (*PTR >> 4 == 0x0E) ? (         \
        3                               \
    ) : (*PTR >> 3 == 0x1E) ? (         \
        4                               \
    ) : (                               \
        0                               \
    )                                   \
)

#define MARY_UTF_Ptr_16_To_Units_16(PTR)    \
(                                           \
    (*PTR < 0xD800 || *PTR > 0xDFFF) ? (    \
        1                                   \
    ) : (                                   \
        2                                   \
    )                                       \
)

////// Encoding / Decoding //////

#define MARY_UTF_Encode_8(UTF_8, CHAR_32, CASE_1, CASE_2, CASE_3, CASE_4)   \
(                                                                           \
    UTF_8.char_32 = CHAR_32,                                                \
                                                                            \
    (UTF_8.char_32 < 0x000080) ? (                                          \
        UTF_8.units = 1,                                                    \
        UTF_8.a = UTF_8.char_32,                                            \
        CASE_1,                                                             \
        UTF_8                                                               \
    ) : (UTF_8.char_32 < 0x000800) ? (                                      \
        UTF_8.units = 2,                                                    \
        UTF_8.a = 0xC0 | UTF_8.char_32 >> 0x06,                             \
        UTF_8.b = 0x80 | UTF_8.char_32 & 0x3F,                              \
        CASE_2,                                                             \
        UTF_8                                                               \
    ) : (UTF_8.char_32 < 0x010000) ? (                                      \
        UTF_8.units = 3,                                                    \
        UTF_8.a = 0xE0 | UTF_8.char_32 >> 0x0C,                             \
        UTF_8.b = 0x80 | UTF_8.char_32 >> 0x06 & 0x3F,                      \
        UTF_8.c = 0x80 | UTF_8.char_32 & 0x3F,                              \
        CASE_3,                                                             \
        UTF_8                                                               \
    ) : (UTF_8.char_32 < 0x110000) ? (                                      \
        UTF_8.units = 4,                                                    \
        UTF_8.a = 0xF0 | UTF_8.char_32 >> 0x12,                             \
        UTF_8.b = 0x80 | UTF_8.char_32 >> 0x0C & 0x3F,                      \
        UTF_8.c = 0x80 | UTF_8.char_32 >> 0x06 & 0x3F,                      \
        UTF_8.d = 0x80 | UTF_8.char_32 & 0x3F,                              \
        CASE_4,                                                             \
        UTF_8                                                               \
    ) : (                                                                   \
        UTF_8                                                               \
    )                                                                       \
)

#define MARY_UTF_Decode_8(UTF_8, PTR)   \
(                                       \
    UTF_8.a = *PTR,                     \
                                        \
    (UTF_8.a >> 7 == 0x00) ? (          \
        UTF_8.units = 1,                \
        UTF_8.char_32 = UTF_8.a,        \
        UTF_8                           \
    ) : (UTF_8.a >> 5 == 0x06) ? (      \
        UTF_8.units = 2,                \
        UTF_8.b = *(PTR + 1),           \
        UTF_8.char_32 =                 \
            (UTF_8.a ^ 0xC0) << 0x06 |  \
            (UTF_8.b ^ 0x80),           \
        UTF_8                           \
    ) : (UTF_8.a >> 4 == 0x0E) ? (      \
        UTF_8.units = 3,                \
        UTF_8.b = *(PTR + 1),           \
        UTF_8.c = *(PTR + 2),           \
        UTF_8.char_32 =                 \
            (UTF_8.a ^ 0xE0) << 0x0C |  \
            (UTF_8.b ^ 0x80) << 0x06 |  \
            (UTF_8.c ^ 0x80),           \
        UTF_8                           \
    ) : (UTF_8.a >> 3 == 0x1E) ? (      \
        UTF_8.units = 4,                \
        UTF_8.b = *(PTR + 1),           \
        UTF_8.c = *(PTR + 2),           \
        UTF_8.d = *(PTR + 3),           \
        UTF_8.char_32 =                 \
            (UTF_8.a ^ 0xF0) << 0x12 |  \
            (UTF_8.b ^ 0x80) << 0x0C |  \
            (UTF_8.c ^ 0x80) << 0x06 |  \
            (UTF_8.d ^ 0x80),           \
        UTF_8                           \
    ) : (                               \
        UTF_8                           \
    )                                   \
)

#define MARY_UTF_Decode_8_Reverse(UTF_8, PTR_END)   \
(                                                   \
    (*((PTR_END) - 1) >> 6 != 0x2) ? (              \
        UTF_8.units = 1,                            \
        UTF_8.a = *((PTR_END) - 1),                 \
        UTF_8.char_32 = UTF_8.a,                    \
        UTF_8                                       \
    ) : (*((PTR_END) - 2) >> 6 != 0x2) ? (          \
        UTF_8.units = 2,                            \
        UTF_8.a = *((PTR_END) - 2),                 \
        UTF_8.b = *((PTR_END) - 1),                 \
        UTF_8.char_32 =                             \
            (UTF_8.a ^ 0xC0) << 0x06 |              \
            (UTF_8.b ^ 0x80),                       \
        UTF_8                                       \
    ) : (*((PTR_END) - 3) >> 6 != 0x2) ? (          \
        UTF_8.units = 3,                            \
        UTF_8.a = *((PTR_END) - 3),                 \
        UTF_8.b = *((PTR_END) - 2),                 \
        UTF_8.c = *((PTR_END) - 1),                 \
        UTF_8.char_32 =                             \
            (UTF_8.a ^ 0xE0) << 0x0C |              \
            (UTF_8.b ^ 0x80) << 0x06 |              \
            (UTF_8.c ^ 0x80),                       \
        UTF_8                                       \
    ) : (*((PTR_END) - 4) >> 6 != 0x2) ? (          \
        UTF_8.units = 4,                            \
        UTF_8.a = *((PTR_END) - 4),                 \
        UTF_8.b = *((PTR_END) - 3),                 \
        UTF_8.c = *((PTR_END) - 2),                 \
        UTF_8.d = *((PTR_END) - 1),                 \
        UTF_8.char_32 =                             \
            (UTF_8.a ^ 0xF0) << 0x12 |              \
            (UTF_8.b ^ 0x80) << 0x0C |              \
            (UTF_8.c ^ 0x80) << 0x06 |              \
            (UTF_8.d ^ 0x80),                       \
        UTF_8                                       \
    ) : (                                           \
        UTF_8                                       \
    )                                               \
)

#define MARY_UTF_Encode_16(UTF_16, CHAR_32, CASE_1, CASE_2)     \
(                                                               \
    UTF_16.char_32 = CHAR_32,                                   \
                                                                \
    (UTF_16.char_32 < 0x10000) ? (                              \
        UTF_16.units = 1,                                       \
        UTF_16.a = UTF_16.char_32,                              \
        CASE_1,                                                 \
        UTF_16                                                  \
    ) : (                                                       \
        UTF_16.units = 2,                                       \
        UTF_16.a = (UTF_16.char_32 - 0x10000 >> 10) + 0xD800,   \
        UTF_16.b = (UTF_16.char_32 - 0x10000 & 0x3FF) + 0xDC00, \
        CASE_2,                                                 \
        UTF_16                                                  \
    )                                                           \
)

#define MARY_UTF_Decode_16(UTF_16, PTR)                 \
(                                                       \
    UTF_16.a = *PTR,                                    \
                                                        \
    (UTF_16.a < 0xD800 || 0xDBFF < UTF_16.a) ? (        \
        UTF_16.units = 1,                               \
        UTF_16.char_32 = UTF_16.a,                      \
        UTF_16                                          \
    ) : (                                               \
        UTF_16.b = *(PTR + 1),                          \
                                                        \
        (UTF_16.b < 0xDC00 || 0xDFFF < UTF_16.b) ? (    \
            UTF_16.units = 1,                           \
            UTF_16.char_32 = UTF_16.a,                  \
            UTF_16                                      \
        ) : (                                           \
            UTF_16.units = 2,                           \
            UTF_16.char_32 =                            \
                (UTF_16.a - 0xD800 << 10) +             \
                (UTF_16.b - 0xDC00) +                   \
                0x10000,                                \
            UTF_16                                      \
        )                                               \
    )                                                   \
)

#define MARY_UTF_Decode_16_Reverse(UTF_16, PTR_END)     \
(                                                       \
    UTF_16.a = *(PTR_END - 1),                          \
                                                        \
    (UTF_16.a < 0xDC00 || 0xDFFF < UTF_16.a) ? (        \
        UTF_16.units = 1,                               \
        UTF_16.char_32 = UTF_16.a,                      \
        UTF_16                                          \
    ) : (                                               \
        UTF_16.b = UTF_16.a,                            \
        UTF_16.a = *(PTR_END - 2),                      \
                                                        \
        (UTF_16.a < 0xD800 || 0xDBFF < UTF_16.a) ? (    \
            UTF_16.units = 1,                           \
            UTF_16.a = UTF_16.b,                        \
            UTF_16.char_32 = UTF_16.a,                  \
            UTF_16                                      \
        ) : (                                           \
            UTF_16.units = 2,                           \
            UTF_16.char_32 =                            \
                (UTF_16.a - 0xD800 << 10) +             \
                (UTF_16.b - 0xDC00) +                   \
                0x10000,                                \
            UTF_16                                      \
        )                                               \
    )                                                   \
)

#define MARY_UTF_Encode_32(UTF_32, CHAR_32, CASE_1) \
(                                                   \
    UTF_32.units = 1,                               \
    UTF_32.char_32 = CHAR_32,                       \
    CASE_1,                                         \
    UTF_32                                          \
)

#define MARY_UTF_Decode_32(UTF_32, PTR)             \
(                                                   \
    UTF_32.units = 1,                               \
    UTF_32.char_32 = *PTR,                          \
    UTF_32                                          \
)

#define MARY_UTF_Decode_32_Reverse(UTF_32, PTR_END) \
(                                                   \
    UTF_32.units = 1,                               \
    UTF_32.char_32 = *(PTR_END - 1),                \
    UTF_32                                          \
)
