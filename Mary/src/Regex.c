#include <Mary/Utils.h>
#include <Mary/Allocator.h>
#include <Mary/Arena.h>
#include <Mary/Hashmap.h>
#include <Mary/UTF.h>
#include <Mary/Regex.h>

MARY_Primitives;

#define ESC '~' // I kinda want the user to be able to choose this, but will backslash work?

uint32_t MARY_Regex_Class_Space[] = U" \n\r\t\f\x00A0\x2002-\x200B";
uint32_t MARY_Regex_Class_Latin[] = U"\x0021-\x007E\x00A1-\x00AC\x00AE-\x024F";
uint32_t MARY_Regex_Class_Latin_Char[] = U"";
uint32_t MARY_Regex_Class_Latin_Char_Lower[] = U"\x0061-\x007A" U"";
uint32_t MARY_Regex_Class_Latin_Char_Upper[] = U"\x0041-\x005A" U"";
uint32_t MARY_Regex_Class_Latin_Punc[] = U"\x0020-\x002F\x003A-\x0040\x005B-\x0060\x007B-\x007E";
uint32_t MARY_Regex_Class_Latin_Numb[] = U"";
uint32_t MARY_Regex_Class_Hebrew[] = U"\x0590-\x05FF\xFB1D-\xFB4F";
uint32_t MARY_Regex_Class_Hebrew_Char[] = U"\x05D0-\x05F2\x05C6\xFB1F-\xFB28\xFB2A-\xFB4F\xFB1D";
uint32_t MARY_Regex_Class_Hebrew_Punc[] = U"";
uint32_t MARY_Regex_Class_Greek[] = U"\x0370-\x03FF\x1F00-\x1FFF";
uint32_t MARY_Regex_Class_Greek_Char[] = U"";
uint32_t MARY_Regex_Class_Greek_Punc[] = U"";

static u32 Class_Phrase_Latin[] = { 'L','a','t','i','n',':' };
static u32 Class_Phrase_Latin_Char[] = { 'L','a','t','i','n',' ','C','h','a','r',':' };
static u32 Class_Phrase_Latin_Punc[] = { 'L','a','t','i','n',' ','P','u','n','c',':' };
static u32 Class_Phrase_Latin_Numb[] = { 'L','a','t','i','n',' ','N','u','m','b',':' };
static u32 Class_Phrase_Hebrew[] = { 'H','e','b','r','e','w',':' };
static u32 Class_Phrase_Hebrew_Char[] = { 'H','e','b','r','e','w',' ', 'C','h','a','r',':' };
static u32 Class_Phrase_Hebrew_Punc[] = { 'H','e','b','r','e','w',' ', 'P','u','n','c',':' };
static u32 Class_Phrase_Greek[] = { 'G','r','e','e','k',':' };
static u32 Class_Phrase_Greek_Char[] = { 'G','r','e','e','k',' ','C','h','a','r',':' };
static u32 Class_Phrase_Greek_Punc[] = { 'G','r','e','e','k',' ','P','u','n','c',':' };

static u64 Class_Phrase_Latin_Units = 6;
static u64 Class_Phrase_Latin_Char_Units = 11;
static u64 Class_Phrase_Latin_Punc_Units = 11;
static u64 Class_Phrase_Latin_Numb_Units = 11;
static u64 Class_Phrase_Hebrew_Units = 7;
static u64 Class_Phrase_Hebrew_Char_Units = 12;
static u64 Class_Phrase_Hebrew_Punc_Units = 12;
static u64 Class_Phrase_Greek_Units = 6;
static u64 Class_Phrase_Greek_Char_Units = 11;
static u64 Class_Phrase_Greek_Punc_Units = 11;

static Mary_Bool_t is_started = MARY_FALSE;
static Mary_Hashmap_t enum_to_classes;

enum Mary_Regex_Flag_e {
    FLAG_GLOBAL,        // 'g'
    FLAG_MULTILINE,     // 'm'
    FLAG_INSENSITIVE    // 'i'
                        // add one that disable /n check on .
};

enum Mary_Regex_Token_e {
    TOKEN_DOT = 0xF0000000, // .
    TOKEN_BAR,              // |
    TOKEN_AND,              // & (implicit)
    TOKEN_QUES,             // ?
    TOKEN_STAR,             // *
    TOKEN_PLUS,             // +
    TOKEN_EXPO,             // ^
    TOKEN_CASH,             // $
    TOKEN_LPAR,             // (
    TOKEN_RPAR,             // )
    TOKEN_LSQU,             // [
    TOKEN_RSQU,             // ]
    TOKEN_BEHIND_POS,       // ?<
    TOKEN_BEHIND_NEG,       // ?<!
    TOKEN_AHEAD_POS,        // ?>
    TOKEN_AHEAD_NEG,        // ?>!
    TOKEN_BOUND_POS,        // ~b
    TOKEN_BOUND_NEG,        // ~B
    TOKEN_CLASS_POS,        // ~s, etc.
    TOKEN_CLASS_NEG         // ~S, etc.
};

enum Mary_Regex_Class_e {
    CLASS_SPACE = 0xF0000000, // ~s ~S
    CLASS_LATIN,              // ~:Latin: ~:!Latin:
    CLASS_LATIN_CHAR,         // ~:Latin Char: ~:!Latin Char:
    CLASS_LATIN_PUNC,         // ~:Latin Punc: ~:!Latin Punc:
    CLASS_LATIN_NUMB,         // ~:Latin Numb: ~:!Latin Numb:
    CLASS_HEBREW,             // ~:Hebrew: ~:!Hebrew:
    CLASS_HEBREW_CHAR,        // ~:Hebrew Char: ~:!Hebrew Char:
    CLASS_HEBREW_PUNC,        // ~:Hebrew Punc: ~:!Hebrew Punc:
    CLASS_GREEK,              // ~:Greek: ~:!Greek:
    CLASS_GREEK_CHAR,         // ~:Greek Char: ~:!Greek Char:
    CLASS_GREEK_PUNC          // ~:Greek Punc: ~:!Greek Punc:
};

enum Mary_Regex_State_e {
    STATE_MATCH,
    STATE_UNICODE,        // 0x0 - 0x10FFFF
    STATE_DOT,            // .
    STATE_CLASS_POS,      // ~s, ~:...:
    STATE_CLASS_NEG,      // ~S, ~:!...:
    STATE_USER_CLASS_POS, // [],
    STATE_USER_CLASS_NEG, // [^]
    STATE_BRANCH,         // *, +, ?, |
    STATE_BEHIND_POS,     // (?<)
    STATE_BEHIND_NEG,     // (?<!)
    STATE_AHEAD_POS,      // (?>)
    STATE_AHEAD_NEG,      // (?>!)
    STATE_BOUND_EXPO,     // ^
    STATE_BOUND_CASH,     // $
    STATE_BOUND_POS,      // ~b
    STATE_BOUND_NEG       // ~B
};

typedef struct System_t System_t;
typedef union Variable_u Variable_u;
typedef Mary_Index_t State_t;
typedef Mary_Vector_t Enum_v;
typedef Mary_Vector_t State_v;
typedef Mary_Vector_t Variable_v;

// Systems should have their own type. but I'm not sure how I would init a variable amount of vectors. needs a macro somehow.
struct System_t {
    Enum_v flags;
    State_v outs;
    Variable_v vars;
};

union Variable_u {
    State_t branch;
    State_t look;
    Mary_Char_32_t unicode;
    Mary_C_String_32_t class;
    Mary_Index_t user_class;
};

typedef struct Fragment_t Fragment_t;

struct Fragment_t {
    State_t state;
    State_v outs;
};

typedef struct Candidate_t Candidate_t;
typedef Mary_Slice_t Match_t;

struct Candidate_t {
    State_t state;
    Mary_Index_t from;
};

static void Mary_Regex_Tokenize(u32 *data, Mary_Size_t units, Mary_Vector_t *v_tokens);
static void Mary_Regex_Compile(Mary_Regex_t *this, Mary_Vector_t *v_tokens);
static void Mary_Regex_Evaluate(System_t *system, Mary_Vector_t *v_frags, u32 operator);
static Mary_Bool_t Mary_Regex_Look_Behind(Mary_Regex_t *this, Mary_String_t *search, Mary_Index_t code_idx, State_t start_state);
static Mary_Bool_t Mary_Regex_Look_Ahead(Mary_Regex_t *this, Mary_String_t *search, Mary_Index_t code_idx, State_t start_state);
static void Mary_Regex_Campaign(Mary_Regex_t *this, Mary_Vector_t *v_old, Mary_Vector_t *v_new, Mary_Vector_t *v_matches,
                                Mary_String_t *search, Mary_Index_t code_idx, Mary_Char_32_t prev_code, Mary_Char_32_t curr_code);

void Mary_Regex_Start()
{
    if (is_started == MARY_TRUE) {
        return;
    }

    Mary_Arena_Start();
    is_started = MARY_TRUE;

    Mary_Enum_32_t class_enum; Mary_C_String_32_t class;
    Mary_Hashmap_Create(&enum_to_classes, sizeof(class_enum), sizeof(class));

    #define HASH(ENUM, STR)                                         \
    MARY_M                                                          \
        class_enum = ENUM, class = STR;                             \
        Mary_Hashmap_Assign(&enum_to_classes, &class_enum, &class); \
    MARY_W

    HASH(CLASS_SPACE, MARY_Regex_Class_Space);
    HASH(CLASS_LATIN, MARY_Regex_Class_Latin);
    HASH(CLASS_LATIN_CHAR, MARY_Regex_Class_Latin_Char);
    HASH(CLASS_LATIN_PUNC, MARY_Regex_Class_Latin_Punc);
    HASH(CLASS_LATIN_NUMB, MARY_Regex_Class_Latin_Numb);
    HASH(CLASS_HEBREW, MARY_Regex_Class_Hebrew);
    HASH(CLASS_HEBREW_CHAR, MARY_Regex_Class_Hebrew_Char);
    HASH(CLASS_HEBREW_PUNC, MARY_Regex_Class_Hebrew_Punc);
    HASH(CLASS_GREEK, MARY_Regex_Class_Greek);
    HASH(CLASS_GREEK_CHAR, MARY_Regex_Class_Greek_Char);
    HASH(CLASS_GREEK_PUNC, MARY_Regex_Class_Greek_Punc);

    #undef HASH
}

void Mary_Regex_Stop()
{
    if (is_started == MARY_FALSE) {
        return;
    }

    Mary_Hashmap_Destroy(&enum_to_classes);

    is_started = MARY_FALSE;
}

void Mary_Regex_Create(Mary_Regex_t *this, Mary_Allocator_t allocator, Mary_String_t *expression, Mary_C_String_8_t flags)
{
    MARY_Assert(is_started == MARY_TRUE, "Mary_Regex has not been started.");
    MARY_Assert(expression->units != 1, "Expression is an empty string.");

    MARY_Arena_In;

    this->allocator = allocator; // may want a TRY. also, how does this work if someone passes FRAME or CHAIN? Maybe I need to do this part outside of Arena.
    Mary_String_Create_From(&this->expression, this->allocator, 32,
                            expression->data, MARY_String_Get_UTF(expression));
    Mary_Vector_Create(&this->classes, this->allocator, sizeof(Mary_Char_32_t), 64); // series of zero-delimited strings, i.e. abc\0def\0

    System_t *system = this->system = Mary_Allocator_Alloc(this->allocator, sizeof(System_t));
    Mary_Vector_Create(&system->flags, this->allocator, sizeof(Mary_Enum_t), 64);
    Mary_Vector_Create(&system->outs, this->allocator, sizeof(State_t), 64);
    Mary_Vector_Create(&system->vars, this->allocator, sizeof(Variable_u), 64);

    this->flags = 0;
    if (flags) {
        while (*flags != 0) {
            Mary_Char_8_t flag_char = *flags;
            if (flag_char == 'g' || flag_char == 'G') {
                MARY_Bit_Set(this->flags, FLAG_GLOBAL);
            } else if (flag_char == 'm' || flag_char == 'M') {
                MARY_Bit_Set(this->flags, FLAG_MULTILINE);
            } else if (flag_char == 'i' || flag_char == 'I') {
                MARY_Bit_Set(this->flags, FLAG_INSENSITIVE);
            }
            ++flags;
        }
    }

    MARY_Vector_Frame(v_tokens, u32, 64);
    Mary_Regex_Tokenize(this->expression.data, this->expression.units, &v_tokens);
    Mary_Regex_Compile(this, &v_tokens); // if we made each machine global, then we could check to see if we have the expression already compiled. might be a good idea.

    MARY_Arena_Out;
}

void Mary_Regex_Destroy(Mary_Regex_t *this)
{
    MARY_Assert(is_started == MARY_TRUE, "Mary_Regex has not been started.");

    System_t *system = this->system;
    Mary_Vector_Destroy(&system->flags);
    Mary_Vector_Destroy(&system->outs);
    Mary_Vector_Destroy(&system->vars);
    Mary_Allocator_Dealloc(this->allocator, system);
    Mary_Vector_Destroy(&this->classes);
    Mary_String_Destroy(&this->expression);
}

static void Mary_Regex_Tokenize(u32 *data, Mary_Size_t units, Mary_Vector_t *v_tokens)
{
    MARY_Assert(data[units - 2] != ESC, "Broken ESC.");

    MARY_Arena_In;

    u32 next;
    MARY_Vector_Frame(v_buffer, u32, 64);
    MARY_Vector_Frame(v_sub_tokens, u32, 64);

    #define ASSERT(THIS_IS_TRUE, ERR)\
        MARY_Assert(THIS_IS_TRUE, "Invalid regex: " ERR)

    #define PUSH(TOKEN)\
        MARY_Vector_Push(v_tokens, u32, TOKEN)

    #define IS_CLASS(CLASS_NAME)                        \
        Mary_Is_Same(it.ptr, Class_Phrase_##CLASS_NAME, \
                     sizeof(Class_Phrase_##CLASS_NAME))

    #define CLASS_UNITS(CLASS_NAME)\
        Class_Phrase_##CLASS_NAME##_Units

    #define TRY_CONCAT                  \
    (                                   \
        next = (it.val == '\0') ? (     \
            '\0'                        \
        ) : (                           \
            *(it.ptr + 1)               \
        ),                              \
        (next != '*'  && next != '+' && \
         next != '?'  && next != '{' && \
         next != ')'  && next != '|' && \
         next != '\0') ? (              \
            PUSH(TOKEN_AND)             \
        ) : (                           \
            0                           \
        )                               \
    )

    MARY_Range(data, u32, 0, units) {
        if (it.val == '.') {
            PUSH(TOKEN_DOT);
            TRY_CONCAT;
        } else if (it.val == '|') {
            PUSH(TOKEN_BAR);
        } else if (it.val == '*') {
            PUSH(TOKEN_STAR);
            TRY_CONCAT;
        } else if (it.val == '+') {
            PUSH(TOKEN_PLUS);
            TRY_CONCAT;
        } else if (it.val == '(') {
            PUSH(TOKEN_LPAR);
        } else if (it.val == ')') {
            PUSH(TOKEN_RPAR);
            TRY_CONCAT;
        } else if (it.val == '^') {
            if (it.idx == 0) {
                PUSH(TOKEN_EXPO);
            } else {
                PUSH(it.val);
            }
            TRY_CONCAT;
        } else if (it.val == '$') {
            // if we put it in conditionally, then we must account for extra )'s between in and '\0'
            //if (*(it.ptr + 1) == '\0') {
            //    PUSH(TOKEN_CASH);
            //} else {
            //    PUSH(it.val);
            //}
            PUSH(TOKEN_CASH);
            TRY_CONCAT;
        } else if (it.val == '[') {
            PUSH(TOKEN_LSQU);
            MARY_Range_Advance(1);
            if (it.val == '^') {
                PUSH(TOKEN_EXPO);
                MARY_Range_Advance(1);
            }
            ASSERT(it.val != ']', "Empty [].");
            while (it.val != ']' && it.val != 0) {
                if (it.val == ESC) {
                    PUSH(*(it.ptr + 1));
                    MARY_Range_Advance(2);
                } else {
                    PUSH(it.val);
                    MARY_Range_Advance(1);
                }
            }
            ASSERT(it.val == ']', "Missing ].");
            PUSH(TOKEN_RSQU);
            TRY_CONCAT;
        } else if (it.val == ESC) {
            MARY_Range_Advance(1);
            if (it.val == 's') {
                PUSH(TOKEN_CLASS_POS);
                PUSH(CLASS_SPACE);
            } else if (it.val == 'S') {
                PUSH(TOKEN_CLASS_NEG);
                PUSH(CLASS_SPACE);
            } else if (it.val == 'b' || it.val == 'B') {
                if (it.val == 'b') {
                    PUSH(TOKEN_BOUND_POS);
                } else {
                    PUSH(TOKEN_BOUND_NEG);
                }
                if (*(it.ptr + 1) == ':') {
                    MARY_Range_Advance(2);
                    if (IS_CLASS(Latin)) {
                        PUSH(CLASS_LATIN_PUNC);
                        MARY_Range_Advance(CLASS_UNITS(Latin));
                    } else if (IS_CLASS(Greek)) {
                        PUSH(CLASS_GREEK_PUNC);
                        MARY_Range_Advance(CLASS_UNITS(Greek));
                    } else if (IS_CLASS(Hebrew)) {
                        PUSH(CLASS_HEBREW_PUNC);
                        MARY_Range_Advance(CLASS_UNITS(Hebrew));
                    } else {
                        ASSERT(0, "Unknown class.");
                    }
                } else {
                    PUSH(CLASS_LATIN_PUNC);
                }
            } else if (it.val == ':') {
                MARY_Range_Advance(1);
                if (it.val == '!') {
                    PUSH(TOKEN_CLASS_NEG);
                    MARY_Range_Advance(1);
                } else {
                    PUSH(TOKEN_CLASS_POS);
                }
                if (it.val == 'L') {
                    if (IS_CLASS(Latin)) {
                        PUSH(CLASS_LATIN);
                        MARY_Range_Advance(CLASS_UNITS(Latin));
                    } else if (IS_CLASS(Latin_Char)) {
                        PUSH(CLASS_LATIN_CHAR);
                        MARY_Range_Advance(CLASS_UNITS(Latin_Char));
                    } else if (IS_CLASS(Latin_Punc)) {
                        PUSH(CLASS_LATIN_PUNC);
                        MARY_Range_Advance(CLASS_UNITS(Latin_Punc));
                    } else if (IS_CLASS(Latin_Numb)) {
                        PUSH(CLASS_LATIN_NUMB);
                        MARY_Range_Advance(CLASS_UNITS(Latin_Numb));
                    } else {
                        ASSERT(0, "Unknown class.");
                    }
                } else if (it.val == 'H') {
                    if (IS_CLASS(Hebrew)) {
                        PUSH(CLASS_HEBREW);
                        MARY_Range_Advance(CLASS_UNITS(Hebrew));
                    } else if (IS_CLASS(Hebrew_Char)) {
                        PUSH(CLASS_HEBREW_CHAR);
                        MARY_Range_Advance(CLASS_UNITS(Hebrew_Char));
                    } else if (IS_CLASS(Hebrew_Punc)) {
                        PUSH(CLASS_HEBREW_PUNC);
                        MARY_Range_Advance(CLASS_UNITS(Hebrew_Punc));
                    } else {
                        ASSERT(0, "Unknown class.");
                    }
                } else if (it.val == 'G') {
                    if (IS_CLASS(Greek)) {
                        PUSH(CLASS_GREEK);
                        MARY_Range_Advance(CLASS_UNITS(Greek));
                    } else if (IS_CLASS(Greek_Char)) {
                        PUSH(CLASS_GREEK_CHAR);
                        MARY_Range_Advance(CLASS_UNITS(Greek_Char));
                    } else if (IS_CLASS(Greek_Punc)) {
                        PUSH(CLASS_GREEK_PUNC);
                        MARY_Range_Advance(CLASS_UNITS(Greek_Punc));
                    } else {
                        ASSERT(0, "Unknown class.");
                    }
                } else {
                    ASSERT(0, "Unknown class.");
                }
            } else {
                PUSH(it.val);
            }
            TRY_CONCAT;
        } else if (it.val == '?') {
            if (*(it.ptr + 1) == '<') {
                MARY_Range_Advance(2);
                if (it.val == '!') {
                    PUSH(TOKEN_BEHIND_NEG);
                    MARY_Range_Advance(1);
                } else {
                    PUSH(TOKEN_BEHIND_POS);
                }
                // tokenize the look behind
                u32 *ptr = it.ptr;
                u64 balance = 1, units = 0;
                while (MARY_TRUE) {
                    ASSERT(it.val != '\0', "Missing ).");
                    if (it.val == ESC) {
                        units += 2;
                        MARY_Range_Advance(2);
                    } else {
                        if (it.val == '(') {
                            ++balance;
                        } else if (it.val == ')') {
                            --balance;
                        }
                        if (balance == 0) {
                            break;
                        } else {
                            ++units;
                            MARY_Range_Advance(1);
                        }
                    }
                }
                MARY_Vector_Empty(&v_sub_tokens);
                Mary_Regex_Tokenize(ptr, units, &v_sub_tokens);
                // reverse TOKEN_AND clusters
                MARY_Vector_Empty(&v_buffer);
                MARY_Vector_Each_Reverse(&v_sub_tokens, u32) {
                    if (it.val != TOKEN_AND) {
                        MARY_Vector_Push(&v_buffer, u32, it.val);
                    } else {
                        while (v_buffer.units != 0) {
                            PUSH(MARY_Vector_Pop(&v_buffer, u32));
                            PUSH(TOKEN_AND);
                        }
                    }
                }
                while (v_buffer.units != 0) {
                    PUSH(MARY_Vector_Pop(&v_buffer, u32));
                }
                PUSH(TOKEN_RPAR);
                TRY_CONCAT;
            } else if (*(it.ptr + 1) == '>') {
                MARY_Range_Advance(1);
                if (*(it.ptr + 1) == '!') {
                    PUSH(TOKEN_AHEAD_NEG);
                    MARY_Range_Advance(1);
                } else {
                    PUSH(TOKEN_AHEAD_POS);
                }
            } else {
                PUSH(TOKEN_QUES);
                TRY_CONCAT;
            }
        } else if (it.val == '{') {
            // read {m,n}
            MARY_Range_Advance(1);
            MARY_Vector_Stack(v_m, u32, 3);
            MARY_Vector_Stack(v_n, u32, 3);
            while (it.val != ',' && it.val != '}' && it.val != '\0') {
                ASSERT(v_m.units < 3, "{m} is too big.");
                ASSERT(it.val >= '0' && it.val <= '9', "{m} is not a number.");
                MARY_Vector_Push(&v_m, u32, it.val);
                MARY_Range_Advance(1);
            }
            ASSERT(it.val == ',' || it.val == '}', "Missing }.");
            while (it.val != '}' && it.val != '\0') {
                ASSERT(v_n.units < 3, "{,n} is too big.");
                ASSERT(it.val >= '0' && it.val <= '9', "{,n} is not a number.");
                MARY_Vector_Push(&v_n, u32, it.val);
                MARY_Range_Advance(1);
            }
            ASSERT(it.val == '}', "Missing }.");
            // convert to numbers
            s64 m = v_m.units ? 0 : -1;
            s64 n = v_n.units ? 0 : -1;
            for (u64 exp = 1; v_m.units; exp *= 10) {
                m += (MARY_Vector_Pop(&v_m, u32) - '0') * exp;
            }
            for (u64 exp = 1; v_n.units; exp *= 10) {
                n += (MARY_Vector_Pop(&v_n, u32) - '0') * exp;
            }
            ASSERT(m != -1, "{} is missing m.");
            ASSERT(n != 0, "{,0} is invalid.");
            ASSERT(n == -1 || n >= m, "{} n is less than m.");
            ASSERT(v_tokens->units != 0, "Dangling {}.");
            // add tokens
            if (m == 0 && n == 1) {
                PUSH(TOKEN_QUES);
            } else if (m == 0 && n == -1) {
                PUSH(TOKEN_STAR);
            } else if (m == 1 && n == -1) {
                PUSH(TOKEN_PLUS);
            } else if (!(m == 1 && n == 1)) {
                // copy previous cluster
                MARY_Vector_Empty(&v_buffer);
                u32 *prev1 = MARY_Vector_Point_Back(v_tokens);
                u32 *prev2 = v_tokens->units > 1 ? prev1 - 1 : 0;
                if (prev2 && *prev1 == TOKEN_RPAR) {
                    // (.){}
                    u32 balance = 1;
                    for (; balance != 0; --prev2) {
                        if (*prev2 == TOKEN_RPAR) {
                            ++balance;
                        } else if (*prev2 == TOKEN_LPAR) {
                            --balance;
                        }
                    }
                    for (; prev2 <= prev1; ++prev2) {
                        MARY_Vector_Push(&v_buffer, u32, *prev2);
                    }
                } else if (prev2 && *prev1 == TOKEN_RSQU) {
                    // [.]{}
                    for (; *prev2 != TOKEN_LSQU; --prev2);
                    for (; prev2 <= prev1; ++prev2) {
                        MARY_Vector_Push(&v_buffer, u32, *prev2);
                    }
                } else if (prev2 && (*prev2 == TOKEN_CLASS_POS ||
                                     *prev2 == TOKEN_CLASS_NEG ||
                                     *prev2 == TOKEN_BOUND_POS ||
                                     *prev2 == TOKEN_BOUND_NEG)) {
                    // ~.{}, ~.:.:{}, ~:.:{}
                    MARY_Vector_Push(&v_buffer, u32, *prev2);
                    MARY_Vector_Push(&v_buffer, u32, *prev1);
                } else {
                    // .{}
                    MARY_Vector_Push(&v_buffer, u32, *prev1);
                }
                // add m
                if (m == 0) {
                    PUSH(TOKEN_QUES);
                } else {
                    for (I i_m = m; i_m > 1; --i_m) {
                        PUSH(TOKEN_AND);
                        for (I i = 0; i < v_buffer.units; ++i) {
                            PUSH(MARY_Vector_At(&v_buffer, u32, i));
                        }
                    }
                }
                // add n
                if (n == -1) {
                    PUSH(TOKEN_AND);
                    for (I i = 0; i < v_buffer.units; ++i) {
                        PUSH(MARY_Vector_At(&v_buffer, u32, i));
                    }
                    PUSH(TOKEN_STAR);
                } else {
                    for (I i_n = n - m; i_n > 0; --i_n) {
                        PUSH(TOKEN_AND);
                        for (I i = 0; i < v_buffer.units; ++i) {
                            PUSH(MARY_Vector_At(&v_buffer, u32, i));
                        }
                        PUSH(TOKEN_QUES);
                    }
                }
            }
            TRY_CONCAT;
        } else {
            PUSH(it.val);
            TRY_CONCAT;
        }
    }

    #undef ASSERT
    #undef PUSH
    #undef IS_CLASS
    #undef CLASS_UNITS
    #undef TRY_CONCAT

    MARY_Arena_Out;
}

/*
* operator precedence =
*   1: +, *, ? 2: (), (?<), (?<!), (?>), (?>!) 3: & 4: |
* operands =
*   unicode, ., [], [^], ^, $, ~., ~::, ~.::
*/

#define STATE_CREATE(FLAG, OUT, VAR_TYPE, VAR)           \
(                                                        \
    MARY_Vector_Push(&system->flags, Mary_Enum_t, FLAG), \
    MARY_Vector_Push(&system->outs, State_t, OUT),       \
    MARY_Vector_Push(&system->vars, VAR_TYPE, VAR),      \
    system->flags.units - 1                              \
)

static void Mary_Regex_Compile(Mary_Regex_t *this, Mary_Vector_t *v_tokens)
{
    MARY_Arena_In;

    System_t *system = this->system;
    MARY_Vector_Frame(v_frags, Fragment_t, 64);
    MARY_Vector_Frame(v_ops, u32, 64);
    u32 op;

    #define EVAL_WHILE(OP_FILTER)                                                            \
    MARY_M                                                                                   \
        while ((op = v_ops.units ? MARY_Vector_At_Back(&v_ops, u32) : 0) && ( OP_FILTER )) { \
            Mary_Regex_Evaluate(system, &v_frags, op);                                       \
            MARY_Vector_Pop(&v_ops, u32);                                                    \
        }                                                                                    \
    MARY_W

    MARY_Range(v_tokens->data, u32, 0, v_tokens->units) {
        if (it.val == TOKEN_PLUS || it.val == TOKEN_STAR || it.val == TOKEN_QUES) {
            EVAL_WHILE(op == TOKEN_PLUS ||
                       op == TOKEN_STAR ||
                       op == TOKEN_QUES);
            MARY_Vector_Push(&v_ops, u32, it.val);
        } else if (it.val == TOKEN_LPAR) {
            EVAL_WHILE(op == TOKEN_PLUS ||
                       op == TOKEN_STAR ||
                       op == TOKEN_QUES);
            MARY_Vector_Push(&v_ops, u32, it.val);
        } else if (it.val == TOKEN_BEHIND_POS || it.val == TOKEN_BEHIND_NEG || it.val == TOKEN_AHEAD_POS || it.val == TOKEN_AHEAD_POS) {
            MARY_Vector_Push(&v_ops, u32, it.val);
        } else if (it.val == TOKEN_RPAR) {
            EVAL_WHILE(op != TOKEN_LPAR);
            MARY_Vector_Pop(&v_ops, u32);
        } else if (it.val == TOKEN_AND) {
            EVAL_WHILE(op == TOKEN_PLUS ||
                       op == TOKEN_STAR ||
                       op == TOKEN_QUES ||
                       op == TOKEN_AND);
            MARY_Vector_Push(&v_ops, u32, it.val);
        } else if (it.val == TOKEN_BAR) {
            EVAL_WHILE(op == TOKEN_PLUS ||
                       op == TOKEN_STAR ||
                       op == TOKEN_QUES ||
                       op == TOKEN_AND ||
                       op == TOKEN_BAR);
            MARY_Vector_Push(&v_ops, u32, it.val);
        } else if (it.val == '\0') {
            Fragment_t frag, match;

            EVAL_WHILE(v_ops.units != 0);

            frag = MARY_Vector_Pop(&v_frags, Fragment_t);

            match.state = STATE_CREATE(STATE_MATCH, ~0, State_t, ~0);

            MARY_Vector_Each(&frag.outs, State_t) {
                MARY_Vector_At(&system->outs, State_t, it.val) = match.state;
            }
            Mary_Vector_Destroy(&frag.outs);

            this->start = frag.state;
        } else if (it.val == TOKEN_EXPO || it.val == TOKEN_CASH) {
            Mary_Enum_t flag = it.val == TOKEN_EXPO ? STATE_BOUND_EXPO : STATE_BOUND_CASH;

            Fragment_t frag;
            frag.state = STATE_CREATE(flag, ~0, State_t, ~0);

            MARY_Vector_Frame_p(&frag.outs, State_t, 1);
            MARY_Vector_Push(&frag.outs, State_t, frag.state);

            MARY_Vector_Push(&v_frags, Fragment_t, frag);
        } else if (it.val == TOKEN_BOUND_POS || it.val == TOKEN_BOUND_NEG || it.val == TOKEN_CLASS_POS || it.val == TOKEN_CLASS_NEG) {
            Mary_Enum_t flag =
                it.val == TOKEN_BOUND_POS ? STATE_BOUND_POS :
                it.val == TOKEN_BOUND_NEG ? STATE_BOUND_NEG :
                it.val == TOKEN_CLASS_POS ? STATE_CLASS_POS :
                STATE_CLASS_NEG;

            MARY_Range_Advance(1);
            Mary_C_String_32_t class;
            Mary_Hashmap_At(&enum_to_classes, &it.val, &class);

            Fragment_t frag;
            frag.state = STATE_CREATE(flag, ~0, Mary_C_String_32_t, class);

            MARY_Vector_Frame_p(&frag.outs, State_t, 1);
            MARY_Vector_Push(&frag.outs, State_t, frag.state);

            MARY_Vector_Push(&v_frags, Fragment_t, frag);
        } else if (it.val == TOKEN_LSQU) {
            MARY_Range_Advance(1);
            Mary_Enum_t flag;
            if (it.val == TOKEN_EXPO) {
                flag = STATE_USER_CLASS_NEG;
                MARY_Range_Advance(1);
            } else {
                flag = STATE_USER_CLASS_POS;
            }

            Mary_Index_t user_class = this->classes.unit;

            while (it.val != TOKEN_RSQU) {
                MARY_Vector_Push(&this->classes, u32, it.val);
                MARY_Range_Advance(1);
            }
            MARY_Vector_Push(&this->classes, u32, '\0');

            Fragment_t frag;
            frag.state = STATE_CREATE(flag, ~0, Mary_Index_t, user_class);

            MARY_Vector_Frame_p(&frag.outs, State_t, 1);
            MARY_Vector_Push(&frag.outs, State_t, frag.state);

            MARY_Vector_Push(&v_frags, Fragment_t, frag);
        } else {
            Mary_Enum_t flag = it.val == TOKEN_DOT ? STATE_DOT : STATE_UNICODE;
            Mary_Char_32_t unicode = it.val == TOKEN_DOT ? ~0 : it.val;

            Fragment_t frag;
            frag.state = STATE_CREATE(flag, ~0, Mary_Char_32_t, unicode);

            MARY_Vector_Frame_p(&frag.outs, State_t, 1);
            MARY_Vector_Push(&frag.outs, State_t, frag.state);

            MARY_Vector_Push(&v_frags, Fragment_t, frag);
        }
    }

    #undef EVAL_WHILE

    MARY_Arena_Out;
}

static void Mary_Regex_Evaluate(System_t *system, Mary_Vector_t *v_frags, u32 op)
{
    MARY_Arena_In;

    if (op == TOKEN_PLUS) {
        Fragment_t frag, out;

        frag = MARY_Vector_Pop(v_frags, Fragment_t);

        out.state = STATE_CREATE(STATE_BRANCH, ~0, State_t, frag.state);

        MARY_Vector_Each(&frag.outs, State_t) {
            MARY_Vector_At(&system->outs, State_t, it.val) = out.state;
        }
        MARY_Vector_Empty(&frag.outs);
        MARY_Vector_Push(&frag.outs, State_t, out.state);

        MARY_Vector_Push(v_frags, Fragment_t, frag);
    } else if (op == TOKEN_STAR) {
        Fragment_t frag, branch;

        branch = MARY_Vector_Pop(v_frags, Fragment_t);

        frag.state = STATE_CREATE(STATE_BRANCH, ~0, State_t, branch.state);

        MARY_Vector_Chain_p(&frag.outs, State_t, 1);
        MARY_Vector_Push(&frag.outs, State_t, frag.state);

        MARY_Vector_Each(&branch.outs, State_t) {
            MARY_Vector_At(&system->outs, State_t, it.val) = frag.state;
        }
        Mary_Vector_Destroy(&branch.outs);

        MARY_Vector_Push(v_frags, Fragment_t, frag);
    } else if (op == TOKEN_QUES) {
        Fragment_t frag, branch;

        branch = MARY_Vector_Pop(v_frags, Fragment_t);

        frag.state = STATE_CREATE(STATE_BRANCH, ~0, State_t, branch.state);

        MARY_Vector_Chain_p(&frag.outs, State_t, 1);
        MARY_Vector_Push(&frag.outs, State_t, frag.state);

        Mary_Vector_Append_Back(&frag.outs, &branch.outs);
        Mary_Vector_Destroy(&branch.outs);

        MARY_Vector_Push(v_frags, Fragment_t, frag);
    } else if (op == TOKEN_AND) {
        Fragment_t frag, out;

        out = MARY_Vector_Pop(v_frags, Fragment_t);
        frag = MARY_Vector_Pop(v_frags, Fragment_t);

        MARY_Vector_Each(&frag.outs, State_t) {
            MARY_Vector_At(&system->outs, State_t, it.val) = out.state;
        }
        Mary_Vector_Destroy(&frag.outs);
        frag.outs = out.outs;

        MARY_Vector_Push(v_frags, Fragment_t, frag);
    } else if (op == TOKEN_BAR) {
        Fragment_t frag, out_a, out_b;

        out_a = MARY_Vector_Pop(v_frags, Fragment_t);
        out_b = MARY_Vector_Pop(v_frags, Fragment_t);

        frag.state = STATE_CREATE(STATE_BRANCH, out_a.state, State_t, out_b.state);

        Mary_Vector_Append_Back(&out_a.outs, &out_b.outs);
        frag.outs = out_a.outs;
        Mary_Vector_Destroy(&out_b.outs);

        MARY_Vector_Push(v_frags, Fragment_t, frag);
    } else if (op == TOKEN_BEHIND_POS || op == TOKEN_BEHIND_NEG || op == TOKEN_AHEAD_POS || op == TOKEN_AHEAD_NEG) {
        Fragment_t paren, match, look;

        paren = MARY_Vector_Pop(v_frags, Fragment_t);

        match.state = STATE_CREATE(STATE_MATCH, ~0, State_t, ~0);

        MARY_Vector_Each(&paren.outs, State_t) {
            MARY_Vector_At(&system->outs, State_t, it.val) = match.state;
        }
        Mary_Vector_Destroy(&paren.outs);

        Mary_Enum_t flag =
            op == TOKEN_BEHIND_POS ? STATE_BEHIND_POS :
            op == TOKEN_BEHIND_NEG ? STATE_BEHIND_NEG :
            op == TOKEN_AHEAD_POS ? STATE_AHEAD_POS :
            STATE_CLASS_NEG;

        look.state = STATE_CREATE(flag, ~0, State_t, paren.state);

        MARY_Vector_Chain_p(&look.outs, State_t, 1);
        MARY_Vector_Push(&look.outs, State_t, look.state);

        MARY_Vector_Push(v_frags, Fragment_t, look);
    } else {
        MARY_Assert(MARY_FALSE, "Unrecognized operator.");
    }

    MARY_Arena_Out;
}

#undef STATE_CREATE

void Mary_Regex_Execute(Mary_Regex_t *this, Mary_String_t *search, Mary_Vector_t *out_matches)
{
    MARY_Assert(is_started == MARY_TRUE, "Mary_Regex has not been started.");
    MARY_Assert(search->data != 0 && search->codes != 0, "Search string not created.");
    MARY_Assert(out_matches->data != 0, "Out_matches needs to manually be created.");

    MARY_Arena_In;

    Mary_Bool_t is_global = MARY_Bit_Is_Set(this->flags, FLAG_GLOBAL);
    Mary_Char_32_t prev_code = ~0;
    MARY_Vector_Frame(v_old_buffer, Candidate_t, 64);
    MARY_Vector_Frame(v_new_buffer, Candidate_t, 64);
    Mary_Vector_t *v_old = &v_old_buffer, *v_new = &v_new_buffer, *v_swap;
    Candidate_t start = { this->start, 0 };

    MARY_String_Each(search) {
        if (is_global || out_matches->units == 0) {
            start.from = it.code_idx;
            MARY_Vector_Push(v_old, Candidate_t, start);
        }

        Mary_Regex_Campaign(this, v_old, v_new, out_matches,
                            search, it.code_idx,
                            prev_code, it.char_32);
        prev_code = it.char_32;
        v_swap = v_old, v_old = v_new, v_new = v_swap;
    }

    // will want to sort matches before sending back. need to develop sort funcs more.

    MARY_Arena_Out;
}

static Mary_Bool_t Mary_Regex_Look_Behind(Mary_Regex_t *this, Mary_String_t *search, Mary_Index_t code_idx, State_t start_state)
{
    MARY_Arena_In;

    Mary_Char_32_t prev_code = ~0;
    MARY_Vector_Frame(v_old_buffer, Candidate_t, 64);
    MARY_Vector_Frame(v_new_buffer, Candidate_t, 64);
    MARY_Vector_Frame(matches, Match_t, 8);
    Mary_Vector_t *v_old = &v_old_buffer, *v_new = &v_new_buffer, *v_swap;
    Candidate_t start = { start_state, 0 }; // .from doesn't matter

    MARY_Vector_Push(v_old, Candidate_t, start);

    MARY_String_Range_Reverse(search, 0, code_idx) {
        if (it.code_idx == 0) {
            prev_code = ~0;
        } else {
            if (it.utf == 8) {
                Mary_UTF_8_t utf_8;
                MARY_UTF_Decode_8(utf_8, it.ptr_u.ptr_8);
                prev_code = utf_8.char_32;
            } else if (it.utf == 16) {
                Mary_UTF_16_t utf_16;
                MARY_UTF_Decode_16(utf_16, it.ptr_u.ptr_16);
                prev_code = utf_16.char_32;
            } else {
                prev_code = *(it.ptr_u.ptr_32 - 1);
            }
        }

        Mary_Regex_Campaign(this, v_old, v_new, &matches,
                            search, it.code_idx,
                            prev_code, it.char_32);

        if (matches.units > 0) {
            MARY_Arena_Return(MARY_TRUE);
        } else if (v_new->units == 0) {
            MARY_Arena_Return(MARY_FALSE);
        }

        v_swap = v_old, v_old = v_new, v_new = v_swap;
    }

    // we need to compensate for a lack of '\0' at the front of str.
    // we need to make sure that this handles bounds right.
    Mary_Regex_Campaign(this, v_old, v_new, &matches,
                        search, 0,
                        ~0, ~0);

    if (matches.units > 0) {
        MARY_Arena_Return(MARY_TRUE);
    }

    MARY_Arena_Return(MARY_FALSE);
}

static Mary_Bool_t Mary_Regex_Look_Ahead(Mary_Regex_t *this, Mary_String_t *search, Mary_Index_t code_idx, State_t start_state)
{
    MARY_Arena_In;

    Mary_Char_32_t prev_code = ~0;
    MARY_Vector_Frame(v_old_buffer, Candidate_t, 64);
    MARY_Vector_Frame(v_new_buffer, Candidate_t, 64);
    MARY_Vector_Frame(matches, Match_t, 8);
    Mary_Vector_t *v_old = &v_old_buffer, *v_new = &v_new_buffer, *v_swap;
    Candidate_t start = { start_state, 0 }; // from doesn't matter

    MARY_Vector_Push(v_old, Candidate_t, start);

    MARY_String_Range(search, code_idx, search->codes) {
        if (it.code_idx == 0) {
            prev_code = ~0;
        } else {
            if (it.utf == 8) {
                Mary_UTF_8_t utf_8;
                MARY_UTF_Decode_8(utf_8, it.ptr_u.ptr_8);
                prev_code = utf_8.char_32;
            } else if (it.utf == 16) {
                Mary_UTF_16_t utf_16;
                MARY_UTF_Decode_16(utf_16, it.ptr_u.ptr_16);
                prev_code = utf_16.char_32;
            } else {
                prev_code = *(it.ptr_u.ptr_32 - 1);
            }
        }

        Mary_Regex_Campaign(this, v_old, v_new, &matches,
                            search, it.code_idx,
                            prev_code, it.char_32);

        if (matches.units > 0) {
            MARY_Arena_Return(MARY_TRUE);
        } else if (v_new->units == 0) {
            MARY_Arena_Return(MARY_FALSE);
        }

        v_swap = v_old, v_old = v_new, v_new = v_swap;
    }

    MARY_Arena_Return(MARY_FALSE);
}

static void Mary_Regex_Campaign(Mary_Regex_t *this,
                                Mary_Vector_t *v_old,
                                Mary_Vector_t *v_new,
                                Mary_Vector_t *v_matches,
                                Mary_String_t *search,
                                Mary_Index_t code_idx,
                                Mary_Char_32_t prev_code,
                                Mary_Char_32_t curr_code)
{
    MARY_Arena_In;

    System_t *system = this->system;
    MARY_Vector_Frame(v_states, State_t, 64);
    Candidate_t old_candidate, new_candidate;
    Match_t match;
    Mary_Enum_t flag;
    State_t state;

    #define PUSH_NEW_CANDIDATE                                                  \
    MARY_M                                                                      \
        new_candidate.state = MARY_Vector_At(&system->outs, State_t, state);    \
        new_candidate.from = old_candidate.from;                                \
        MARY_Vector_Push(v_new, Candidate_t, new_candidate);                    \
    MARY_W

    #define PUSH_NEXT_STATE                                                                     \
    MARY_M                                                                                      \
        MARY_Vector_Push(&v_states, State_t, MARY_Vector_At(&system->outs, State_t, state));    \
    MARY_W

    MARY_Vector_Each(v_old, Candidate_t) {
        old_candidate = it.val;
        MARY_Vector_Push(&v_states, State_t, old_candidate.state);

        MARY_Vector_Each(&v_states, State_t) {
            state = it.val;
            flag = MARY_Vector_At(&system->flags, Mary_Enum_t, state);
            if (flag == STATE_MATCH) {
                match.from = old_candidate.from;
                match.to_exclusive = code_idx;
                MARY_Vector_Push(v_matches, Match_t, match);
            } else if (flag == STATE_BRANCH) {
                MARY_Vector_Push(&v_states, State_t, MARY_Vector_At(&system->outs, State_t, state));
                MARY_Vector_Push(&v_states, State_t, MARY_Vector_At(&system->vars, State_t, state));
            } else if (flag == STATE_BEHIND_POS) {
                if (code_idx != 0) {
                    State_t start_state = MARY_Vector_At(&system->vars, State_t, state);
                    if (Mary_Regex_Look_Behind(this, search, code_idx, start_state)) {
                        PUSH_NEXT_STATE;
                    }
                }
            } else if (flag == STATE_BEHIND_NEG) {
                if (code_idx != 0) {
                    State_t start_state = MARY_Vector_At(&system->vars, State_t, state);
                    if (!Mary_Regex_Look_Behind(this, search, code_idx, start_state)) {
                        PUSH_NEXT_STATE;
                    }
                } else {
                    PUSH_NEXT_STATE;
                }
            } else if (flag == STATE_AHEAD_POS) {
                if (curr_code != '\0') {
                    State_t start_state = MARY_Vector_At(&system->vars, State_t, state);
                    if (Mary_Regex_Look_Ahead(this, search, code_idx, start_state)) {
                        PUSH_NEXT_STATE;
                    }
                }
            } else if (flag == STATE_AHEAD_NEG) {
                if (curr_code != '\0') {
                    State_t start_state = MARY_Vector_At(&system->vars, State_t, state);
                    if (!Mary_Regex_Look_Ahead(this, search, code_idx, start_state)) {
                        PUSH_NEXT_STATE;
                    }
                } else {
                    PUSH_NEXT_STATE;
                }
            } else if (flag == STATE_BOUND_EXPO) {
                Mary_Bool_t is_multiline = MARY_Bit_Is_Set(this->flags, FLAG_MULTILINE);
                if (code_idx == 0 || (is_multiline && (prev_code == '\n' || prev_code == '\r'))) {
                    PUSH_NEXT_STATE;
                }
            } else if (flag == STATE_BOUND_CASH) {
                Mary_Bool_t is_multiline = MARY_Bit_Is_Set(this->flags, FLAG_MULTILINE);
                if (curr_code == '\0' || (is_multiline && (curr_code == '\n' || curr_code == '\r'))) {
                    PUSH_NEXT_STATE;
                }
            } else if (flag == STATE_BOUND_POS) {
                Mary_Bool_t prev_exists = code_idx != 0;
                Mary_Bool_t curr_exists = curr_code != '\0';
                Mary_Bool_t prev_is_bound = 0, curr_is_bound = 0;
                Mary_C_String_32_t class = MARY_Vector_At(&system->vars, Mary_C_String_32_t, state);
                if (prev_exists) {
                    MARY_Regex_Class_Has(class, prev_code, prev_is_bound);
                }
                if (curr_exists) {
                    MARY_Regex_Class_Has(class, curr_code, curr_is_bound);
                }
                if ((!prev_exists && curr_exists && !curr_is_bound) ||
                    (!curr_exists && prev_exists && !prev_is_bound) ||
                    (prev_exists && curr_exists && prev_is_bound && !curr_is_bound) ||
                    (prev_exists && curr_exists && !prev_is_bound && curr_is_bound)) {
                    PUSH_NEXT_STATE;
                }
            } else if (flag == STATE_BOUND_NEG) {
                Mary_Bool_t prev_exists = code_idx != 0;
                Mary_Bool_t curr_exists = curr_code != '\0';
                Mary_Bool_t prev_is_bound = 0, curr_is_bound = 0;
                Mary_C_String_32_t class = MARY_Vector_At(&system->vars, Mary_C_String_32_t, state);
                if (prev_exists) {
                    MARY_Regex_Class_Has(class, prev_code, prev_is_bound);
                }
                if (curr_exists) {
                    MARY_Regex_Class_Has(class, curr_code, curr_is_bound);
                }
                if ((!prev_exists && curr_exists && curr_is_bound) ||
                    (!curr_exists && prev_exists && prev_is_bound) ||
                    (prev_exists && curr_exists && prev_is_bound && curr_is_bound) ||
                    (prev_exists && curr_exists && !prev_is_bound && !curr_is_bound)) {
                    PUSH_NEXT_STATE;
                }
            } else if (flag == STATE_CLASS_POS) {
                Mary_C_String_32_t class = MARY_Vector_At(&system->vars, Mary_C_String_32_t, state);
                Mary_Bool_t is_in_class;
                MARY_Regex_Class_Has(class, curr_code, is_in_class);
                if (is_in_class) {
                    PUSH_NEW_CANDIDATE;
                }
            } else if (flag == STATE_CLASS_NEG) {
                Mary_C_String_32_t class = MARY_Vector_At(&system->vars, Mary_C_String_32_t, state);
                Mary_Bool_t is_in_class;
                MARY_Regex_Class_Has(class, curr_code, is_in_class);
                if (!is_in_class) {
                    PUSH_NEW_CANDIDATE;
                }
            } else if (flag == STATE_USER_CLASS_POS) {
                Mary_Index_t class_idx = MARY_Vector_At(&system->vars, Mary_Index_t, state);
                Mary_C_String_32_t class = MARY_Vector_Point(&this->classes, class_idx);
                Mary_Bool_t is_in_class;
                MARY_Regex_Class_Has(class, curr_code, is_in_class);
                if (is_in_class) {
                    PUSH_NEW_CANDIDATE;
                }
            } else if (flag == STATE_USER_CLASS_NEG) {
                Mary_Index_t class_idx = MARY_Vector_At(&system->vars, Mary_Index_t, state);
                Mary_C_String_32_t class = MARY_Vector_Point(&this->classes, class_idx);
                Mary_Bool_t is_in_class;
                MARY_Regex_Class_Has(class, curr_code, is_in_class);
                if (!is_in_class) {
                    PUSH_NEW_CANDIDATE;
                }
            } else if (flag == STATE_DOT) {
                if (curr_code != '\n') {
                    PUSH_NEW_CANDIDATE;
                }
            } else if (flag == STATE_UNICODE) {
                if (curr_code == MARY_Vector_At(&system->vars, Mary_Char_32_t, state)) {
                    PUSH_NEW_CANDIDATE;
                }
            }
        }

        MARY_Vector_Empty(&v_states);
    }

    MARY_Vector_Empty(v_old);

    #undef PUSH_NEW_CANDIDATE
    #undef PUSH_NEXT_STATE

    MARY_Arena_Out;
}
