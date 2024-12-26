#pragma once

#define ASSERT_SIZE(type, size) static_assert(sizeof(type) == size, "sizeof("#type")" " != " #size)

#define BITMASK_ENUM_OPERATORS(type) \
inline bool operator&(type lhs, type rhs) \
{ \
    return (uint32_t(lhs) & uint32_t(rhs)); \
} \
inline type operator|(type lhs, type rhs) \
{ \
    return type(uint32_t(lhs) | uint32_t(rhs)); \
} \
inline type operator^(type lhs, type rhs) \
{ \
    return type(uint32_t(lhs) ^ uint32_t(rhs)); \
} \
inline type operator~(type lhs) \
{ \
    return type(~uint32_t(lhs)); \
} \
inline type operator&=(type& lhs, type rhs) \
{ \
    lhs = type(uint32_t(lhs) & uint32_t(rhs)); \
    return lhs; \
} \
inline type operator|=(type& lhs, type rhs) \
{ \
    lhs = type(uint32_t(lhs) | uint32_t(rhs)); \
    return lhs; \
} \
inline type operator^=(type& lhs, type rhs) \
{ \
    lhs = type(uint32_t(lhs) ^ uint32_t(rhs)); \
    return lhs; \
}

#define BASIC_ENUM_REFLECTION_WRITE_STRING(arg) #arg, 

#define BASIC_ENUM_REFLECTION_FIRST_INDEX_1(underlyingType, firstValue, ...) \
    enum Enum : underlyingType \
    { \
        firstValue = 1, __VA_ARGS__, COUNT \
    }; \
    static const char* EnumToString(Enum type) \
    { \
        if((size_t)type <= std::size(msNames)) \
            return msNames[(uint32_t)type]; \
        else \
            return "INVALID"; \
    } \
    \
    static Enum StringToEnum(const char* string) \
    { \
    for(size_t i = 0; i < std::size(msNames); i++) \
    { \
        if(strcmp(string, msNames[i]) == 0) \
            return (Enum)i; \
    } \
    return Enum::COUNT; \
    } \
    \
private: \
    static constexpr const char* msNames[] \
    { \
        "INVALID", #firstValue, FOR_EACH(BASIC_ENUM_REFLECTION_WRITE_STRING, __VA_ARGS__) \
    };

#define BASIC_ENUM_REFLECTION(underlyingType, ...) \
    enum Enum : underlyingType \
    { \
        __VA_ARGS__, COUNT \
    }; \
    static const char* EnumToString(Enum type) \
    { \
        if((size_t)type <= std::size(msNames)) \
            return msNames[(uint32_t)type]; \
        else \
            return "INVALID"; \
    } \
    \
    static Enum StringToEnum(const char* string) \
    { \
    for(size_t i = 0; i < std::size(msNames); i++) \
    { \
        if(strcmp(string, msNames[i]) == 0) \
            return (Enum)i; \
    } \
    return Enum::COUNT; \
    } \
    \
private: \
    static constexpr const char* msNames[] \
    { \
        FOR_EACH(BASIC_ENUM_REFLECTION_WRITE_STRING, __VA_ARGS__) \
    };


//from https://stackoverflow.com/a/11994395
#define FOR_EACH_1(macro, arg) macro(arg)
#define FOR_EACH_2(macro, arg, ...) macro(arg) FOR_EACH_1(macro, __VA_ARGS__)
#define FOR_EACH_3(macro, arg, ...) macro(arg) FOR_EACH_2(macro, __VA_ARGS__)
#define FOR_EACH_4(macro, arg, ...) macro(arg) FOR_EACH_3(macro, __VA_ARGS__)
#define FOR_EACH_5(macro, arg, ...) macro(arg) FOR_EACH_4(macro, __VA_ARGS__)
#define FOR_EACH_6(macro, arg, ...) macro(arg) FOR_EACH_5(macro, __VA_ARGS__)
#define FOR_EACH_7(macro, arg, ...) macro(arg) FOR_EACH_6(macro, __VA_ARGS__)
#define FOR_EACH_8(macro, arg, ...) macro(arg) FOR_EACH_7(macro, __VA_ARGS__)
#define FOR_EACH_9(macro, arg, ...) macro(arg) FOR_EACH_8(macro, __VA_ARGS__)
#define FOR_EACH_10(macro, arg, ...) macro(arg) FOR_EACH_9(macro, __VA_ARGS__)
#define FOR_EACH_11(macro, arg, ...) macro(arg) FOR_EACH_10(macro, __VA_ARGS__)
#define FOR_EACH_12(macro, arg, ...) macro(arg) FOR_EACH_11(macro, __VA_ARGS__)
#define FOR_EACH_13(macro, arg, ...) macro(arg) FOR_EACH_12(macro, __VA_ARGS__)
#define FOR_EACH_14(macro, arg, ...) macro(arg) FOR_EACH_13(macro, __VA_ARGS__)
#define FOR_EACH_15(macro, arg, ...) macro(arg) FOR_EACH_14(macro, __VA_ARGS__)
#define FOR_EACH_16(macro, arg, ...) macro(arg) FOR_EACH_15(macro, __VA_ARGS__)
#define FOR_EACH_17(macro, arg, ...) macro(arg) FOR_EACH_16(macro, __VA_ARGS__)
#define FOR_EACH_18(macro, arg, ...) macro(arg) FOR_EACH_17(macro, __VA_ARGS__)
#define FOR_EACH_19(macro, arg, ...) macro(arg) FOR_EACH_18(macro, __VA_ARGS__)
#define FOR_EACH_20(macro, arg, ...) macro(arg) FOR_EACH_19(macro, __VA_ARGS__)
#define FOR_EACH_21(macro, arg, ...) macro(arg) FOR_EACH_20(macro, __VA_ARGS__)
#define FOR_EACH_22(macro, arg, ...) macro(arg) FOR_EACH_21(macro, __VA_ARGS__)
#define FOR_EACH_23(macro, arg, ...) macro(arg) FOR_EACH_22(macro, __VA_ARGS__)
#define FOR_EACH_24(macro, arg, ...) macro(arg) FOR_EACH_23(macro, __VA_ARGS__)
#define FOR_EACH_25(macro, arg, ...) macro(arg) FOR_EACH_24(macro, __VA_ARGS__)
#define FOR_EACH_26(macro, arg, ...) macro(arg) FOR_EACH_25(macro, __VA_ARGS__)
#define FOR_EACH_27(macro, arg, ...) macro(arg) FOR_EACH_26(macro, __VA_ARGS__)
#define FOR_EACH_28(macro, arg, ...) macro(arg) FOR_EACH_27(macro, __VA_ARGS__)
#define FOR_EACH_29(macro, arg, ...) macro(arg) FOR_EACH_28(macro, __VA_ARGS__)
#define FOR_EACH_30(macro, arg, ...) macro(arg) FOR_EACH_29(macro, __VA_ARGS__)
#define FOR_EACH_31(macro, arg, ...) macro(arg) FOR_EACH_30(macro, __VA_ARGS__)
#define FOR_EACH_32(macro, arg, ...) macro(arg) FOR_EACH_31(macro, __VA_ARGS__)
#define FOR_EACH_33(macro, arg, ...) macro(arg) FOR_EACH_32(macro, __VA_ARGS__)
#define FOR_EACH_34(macro, arg, ...) macro(arg) FOR_EACH_33(macro, __VA_ARGS__)
#define FOR_EACH_35(macro, arg, ...) macro(arg) FOR_EACH_34(macro, __VA_ARGS__)
#define FOR_EACH_36(macro, arg, ...) macro(arg) FOR_EACH_35(macro, __VA_ARGS__)
#define FOR_EACH_37(macro, arg, ...) macro(arg) FOR_EACH_36(macro, __VA_ARGS__)
#define FOR_EACH_38(macro, arg, ...) macro(arg) FOR_EACH_37(macro, __VA_ARGS__)
#define FOR_EACH_39(macro, arg, ...) macro(arg) FOR_EACH_38(macro, __VA_ARGS__)
#define FOR_EACH_40(macro, arg, ...) macro(arg) FOR_EACH_39(macro, __VA_ARGS__)
#define FOR_EACH_41(macro, arg, ...) macro(arg) FOR_EACH_40(macro, __VA_ARGS__)
#define FOR_EACH_42(macro, arg, ...) macro(arg) FOR_EACH_41(macro, __VA_ARGS__)
#define FOR_EACH_43(macro, arg, ...) macro(arg) FOR_EACH_42(macro, __VA_ARGS__)
#define FOR_EACH_44(macro, arg, ...) macro(arg) FOR_EACH_43(macro, __VA_ARGS__)
#define FOR_EACH_45(macro, arg, ...) macro(arg) FOR_EACH_44(macro, __VA_ARGS__)
#define FOR_EACH_46(macro, arg, ...) macro(arg) FOR_EACH_45(macro, __VA_ARGS__)
#define FOR_EACH_47(macro, arg, ...) macro(arg) FOR_EACH_46(macro, __VA_ARGS__)
#define FOR_EACH_48(macro, arg, ...) macro(arg) FOR_EACH_47(macro, __VA_ARGS__)
#define FOR_EACH_49(macro, arg, ...) macro(arg) FOR_EACH_48(macro, __VA_ARGS__)
#define FOR_EACH_50(macro, arg, ...) macro(arg) FOR_EACH_49(macro, __VA_ARGS__)
#define FOR_EACH_51(macro, arg, ...) macro(arg) FOR_EACH_50(macro, __VA_ARGS__)
#define FOR_EACH_52(macro, arg, ...) macro(arg) FOR_EACH_51(macro, __VA_ARGS__)
#define FOR_EACH_53(macro, arg, ...) macro(arg) FOR_EACH_52(macro, __VA_ARGS__)
#define FOR_EACH_54(macro, arg, ...) macro(arg) FOR_EACH_53(macro, __VA_ARGS__)
#define FOR_EACH_55(macro, arg, ...) macro(arg) FOR_EACH_54(macro, __VA_ARGS__)
#define FOR_EACH_56(macro, arg, ...) macro(arg) FOR_EACH_55(macro, __VA_ARGS__)
#define FOR_EACH_57(macro, arg, ...) macro(arg) FOR_EACH_56(macro, __VA_ARGS__)
#define FOR_EACH_58(macro, arg, ...) macro(arg) FOR_EACH_57(macro, __VA_ARGS__)
#define FOR_EACH_59(macro, arg, ...) macro(arg) FOR_EACH_58(macro, __VA_ARGS__)
#define FOR_EACH_60(macro, arg, ...) macro(arg) FOR_EACH_59(macro, __VA_ARGS__)
#define FOR_EACH_61(macro, arg, ...) macro(arg) FOR_EACH_60(macro, __VA_ARGS__)
#define FOR_EACH_62(macro, arg, ...) macro(arg) FOR_EACH_61(macro, __VA_ARGS__)
#define FOR_EACH_63(macro, arg, ...) macro(arg) FOR_EACH_62(macro, __VA_ARGS__)
#define FOR_EACH_64(macro, arg, ...) macro(arg) FOR_EACH_63(macro, __VA_ARGS__)

#define GET_MACRO(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, NAME, ...) NAME
#define FOR_EACH(macro, ...) GET_MACRO(__VA_ARGS__, FOR_EACH_64, FOR_EACH_63, FOR_EACH_62, FOR_EACH_61, FOR_EACH_60, FOR_EACH_59, FOR_EACH_58, FOR_EACH_57, FOR_EACH_56, FOR_EACH_55, FOR_EACH_54, FOR_EACH_53, FOR_EACH_52, FOR_EACH_51, FOR_EACH_50, FOR_EACH_49, FOR_EACH_48, FOR_EACH_47, FOR_EACH_46, FOR_EACH_45, FOR_EACH_44, FOR_EACH_43, FOR_EACH_42, FOR_EACH_41, FOR_EACH_40, FOR_EACH_39, FOR_EACH_38, FOR_EACH_37, FOR_EACH_36, FOR_EACH_35, FOR_EACH_34, FOR_EACH_33, FOR_EACH_32, FOR_EACH_31, FOR_EACH_30, FOR_EACH_29, FOR_EACH_28, FOR_EACH_27, FOR_EACH_26, FOR_EACH_25, FOR_EACH_24, FOR_EACH_23, FOR_EACH_22, FOR_EACH_21, FOR_EACH_20, FOR_EACH_19, FOR_EACH_18, FOR_EACH_17, FOR_EACH_16, FOR_EACH_15, FOR_EACH_14, FOR_EACH_13, FOR_EACH_12, FOR_EACH_11, FOR_EACH_10, FOR_EACH_9, FOR_EACH_8, FOR_EACH_7, FOR_EACH_6, FOR_EACH_5, FOR_EACH_4, FOR_EACH_3, FOR_EACH_2, FOR_EACH_1)(macro, __VA_ARGS__)