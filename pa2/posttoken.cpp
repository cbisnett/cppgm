// (C) 2013 CPPGM Foundation www.cppgm.org.  All rights reserved.

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <cassert>
#include <memory>
#include <cstring>
#include <cstdint>
#include <climits>
#include <cfloat>
#include <map>

#include "pptoken.h"

using namespace std;

// See 3.9.1: Fundamental Types
enum EFundamentalType
{
    FT_INVALID = 0,

    // 3.9.1.2
    FT_SIGNED_CHAR,
    FT_SHORT_INT,
    FT_INT,
    FT_LONG_INT,
    FT_LONG_LONG_INT,

    // 3.9.1.3
    FT_UNSIGNED_CHAR,
    FT_UNSIGNED_SHORT_INT,
    FT_UNSIGNED_INT,
    FT_UNSIGNED_LONG_INT,
    FT_UNSIGNED_LONG_LONG_INT,

    // 3.9.1.1 / 3.9.1.5
    FT_WCHAR_T,
    FT_CHAR,
    FT_CHAR16_T,
    FT_CHAR32_T,

    // 3.9.1.6
    FT_BOOL,

    // 3.9.1.8
    FT_FLOAT,
    FT_DOUBLE,
    FT_LONG_DOUBLE,

    // 3.9.1.9
    FT_VOID,

    // 3.9.1.10
    FT_NULLPTR_T
};

// FundamentalTypeOf: convert fundamental type T to EFundamentalType
// for example: `FundamentalTypeOf<long int>()` will return `FT_LONG_INT`
template<typename T> constexpr EFundamentalType FundamentalTypeOf();
template<> constexpr EFundamentalType FundamentalTypeOf<signed char>() { return FT_SIGNED_CHAR; }
template<> constexpr EFundamentalType FundamentalTypeOf<short int>() { return FT_SHORT_INT; }
template<> constexpr EFundamentalType FundamentalTypeOf<int>() { return FT_INT; }
template<> constexpr EFundamentalType FundamentalTypeOf<long int>() { return FT_LONG_INT; }
template<> constexpr EFundamentalType FundamentalTypeOf<long long int>() { return FT_LONG_LONG_INT; }
template<> constexpr EFundamentalType FundamentalTypeOf<unsigned char>() { return FT_UNSIGNED_CHAR; }
template<> constexpr EFundamentalType FundamentalTypeOf<unsigned short int>() { return FT_UNSIGNED_SHORT_INT; }
template<> constexpr EFundamentalType FundamentalTypeOf<unsigned int>() { return FT_UNSIGNED_INT; }
template<> constexpr EFundamentalType FundamentalTypeOf<unsigned long int>() { return FT_UNSIGNED_LONG_INT; }
template<> constexpr EFundamentalType FundamentalTypeOf<unsigned long long int>() { return FT_UNSIGNED_LONG_LONG_INT; }
template<> constexpr EFundamentalType FundamentalTypeOf<wchar_t>() { return FT_WCHAR_T; }
template<> constexpr EFundamentalType FundamentalTypeOf<char>() { return FT_CHAR; }
template<> constexpr EFundamentalType FundamentalTypeOf<char16_t>() { return FT_CHAR16_T; }
template<> constexpr EFundamentalType FundamentalTypeOf<char32_t>() { return FT_CHAR32_T; }
template<> constexpr EFundamentalType FundamentalTypeOf<bool>() { return FT_BOOL; }
template<> constexpr EFundamentalType FundamentalTypeOf<float>() { return FT_FLOAT; }
template<> constexpr EFundamentalType FundamentalTypeOf<double>() { return FT_DOUBLE; }
template<> constexpr EFundamentalType FundamentalTypeOf<long double>() { return FT_LONG_DOUBLE; }
template<> constexpr EFundamentalType FundamentalTypeOf<void>() { return FT_VOID; }
template<> constexpr EFundamentalType FundamentalTypeOf<nullptr_t>() { return FT_NULLPTR_T; }

// convert EFundamentalType to a source code
const map<EFundamentalType, string> FundamentalTypeToStringMap
{
    {FT_SIGNED_CHAR, "signed char"},
    {FT_SHORT_INT, "short int"},
    {FT_INT, "int"},
    {FT_LONG_INT, "long int"},
    {FT_LONG_LONG_INT, "long long int"},
    {FT_UNSIGNED_CHAR, "unsigned char"},
    {FT_UNSIGNED_SHORT_INT, "unsigned short int"},
    {FT_UNSIGNED_INT, "unsigned int"},
    {FT_UNSIGNED_LONG_INT, "unsigned long int"},
    {FT_UNSIGNED_LONG_LONG_INT, "unsigned long long int"},
    {FT_WCHAR_T, "wchar_t"},
    {FT_CHAR, "char"},
    {FT_CHAR16_T, "char16_t"},
    {FT_CHAR32_T, "char32_t"},
    {FT_BOOL, "bool"},
    {FT_FLOAT, "float"},
    {FT_DOUBLE, "double"},
    {FT_LONG_DOUBLE, "long double"},
    {FT_VOID, "void"},
    {FT_NULLPTR_T, "nullptr_t"}
};

// token type enum for `simples`
enum ETokenType
{
    // keywords
    KW_ALIGNAS,
    KW_ALIGNOF,
    KW_ASM,
    KW_AUTO,
    KW_BOOL,
    KW_BREAK,
    KW_CASE,
    KW_CATCH,
    KW_CHAR,
    KW_CHAR16_T,
    KW_CHAR32_T,
    KW_CLASS,
    KW_CONST,
    KW_CONSTEXPR,
    KW_CONST_CAST,
    KW_CONTINUE,
    KW_DECLTYPE,
    KW_DEFAULT,
    KW_DELETE,
    KW_DO,
    KW_DOUBLE,
    KW_DYNAMIC_CAST,
    KW_ELSE,
    KW_ENUM,
    KW_EXPLICIT,
    KW_EXPORT,
    KW_EXTERN,
    KW_FALSE,
    KW_FLOAT,
    KW_FOR,
    KW_FRIEND,
    KW_GOTO,
    KW_IF,
    KW_INLINE,
    KW_INT,
    KW_LONG,
    KW_MUTABLE,
    KW_NAMESPACE,
    KW_NEW,
    KW_NOEXCEPT,
    KW_NULLPTR,
    KW_OPERATOR,
    KW_PRIVATE,
    KW_PROTECTED,
    KW_PUBLIC,
    KW_REGISTER,
    KW_REINTERPET_CAST,
    KW_RETURN,
    KW_SHORT,
    KW_SIGNED,
    KW_SIZEOF,
    KW_STATIC,
    KW_STATIC_ASSERT,
    KW_STATIC_CAST,
    KW_STRUCT,
    KW_SWITCH,
    KW_TEMPLATE,
    KW_THIS,
    KW_THREAD_LOCAL,
    KW_THROW,
    KW_TRUE,
    KW_TRY,
    KW_TYPEDEF,
    KW_TYPEID,
    KW_TYPENAME,
    KW_UNION,
    KW_UNSIGNED,
    KW_USING,
    KW_VIRTUAL,
    KW_VOID,
    KW_VOLATILE,
    KW_WCHAR_T,
    KW_WHILE,

    // operators/punctuation
    OP_LBRACE,
    OP_RBRACE,
    OP_LSQUARE,
    OP_RSQUARE,
    OP_LPAREN,
    OP_RPAREN,
    OP_BOR,
    OP_XOR,
    OP_COMPL,
    OP_AMP,
    OP_LNOT,
    OP_SEMICOLON,
    OP_COLON,
    OP_DOTS,
    OP_QMARK,
    OP_COLON2,
    OP_DOT,
    OP_DOTSTAR,
    OP_PLUS,
    OP_MINUS,
    OP_STAR,
    OP_DIV,
    OP_MOD,
    OP_ASS,
    OP_LT,
    OP_GT,
    OP_PLUSASS,
    OP_MINUSASS,
    OP_STARASS,
    OP_DIVASS,
    OP_MODASS,
    OP_XORASS,
    OP_BANDASS,
    OP_BORASS,
    OP_LSHIFT,
    OP_RSHIFT,
    OP_RSHIFTASS,
    OP_LSHIFTASS,
    OP_EQ,
    OP_NE,
    OP_LE,
    OP_GE,
    OP_LAND,
    OP_LOR,
    OP_INC,
    OP_DEC,
    OP_COMMA,
    OP_ARROWSTAR,
    OP_ARROW,
};

// StringToETokenTypeMap map of `simple` `preprocessing-tokens` to ETokenType
const unordered_map<string, ETokenType> StringToTokenTypeMap =
{
    // keywords
    {"alignas", KW_ALIGNAS},
    {"alignof", KW_ALIGNOF},
    {"asm", KW_ASM},
    {"auto", KW_AUTO},
    {"bool", KW_BOOL},
    {"break", KW_BREAK},
    {"case", KW_CASE},
    {"catch", KW_CATCH},
    {"char", KW_CHAR},
    {"char16_t", KW_CHAR16_T},
    {"char32_t", KW_CHAR32_T},
    {"class", KW_CLASS},
    {"const", KW_CONST},
    {"constexpr", KW_CONSTEXPR},
    {"const_cast", KW_CONST_CAST},
    {"continue", KW_CONTINUE},
    {"decltype", KW_DECLTYPE},
    {"default", KW_DEFAULT},
    {"delete", KW_DELETE},
    {"do", KW_DO},
    {"double", KW_DOUBLE},
    {"dynamic_cast", KW_DYNAMIC_CAST},
    {"else", KW_ELSE},
    {"enum", KW_ENUM},
    {"explicit", KW_EXPLICIT},
    {"export", KW_EXPORT},
    {"extern", KW_EXTERN},
    {"false", KW_FALSE},
    {"float", KW_FLOAT},
    {"for", KW_FOR},
    {"friend", KW_FRIEND},
    {"goto", KW_GOTO},
    {"if", KW_IF},
    {"inline", KW_INLINE},
    {"int", KW_INT},
    {"long", KW_LONG},
    {"mutable", KW_MUTABLE},
    {"namespace", KW_NAMESPACE},
    {"new", KW_NEW},
    {"noexcept", KW_NOEXCEPT},
    {"nullptr", KW_NULLPTR},
    {"operator", KW_OPERATOR},
    {"private", KW_PRIVATE},
    {"protected", KW_PROTECTED},
    {"public", KW_PUBLIC},
    {"register", KW_REGISTER},
    {"reinterpret_cast", KW_REINTERPET_CAST},
    {"return", KW_RETURN},
    {"short", KW_SHORT},
    {"signed", KW_SIGNED},
    {"sizeof", KW_SIZEOF},
    {"static", KW_STATIC},
    {"static_assert", KW_STATIC_ASSERT},
    {"static_cast", KW_STATIC_CAST},
    {"struct", KW_STRUCT},
    {"switch", KW_SWITCH},
    {"template", KW_TEMPLATE},
    {"this", KW_THIS},
    {"thread_local", KW_THREAD_LOCAL},
    {"throw", KW_THROW},
    {"true", KW_TRUE},
    {"try", KW_TRY},
    {"typedef", KW_TYPEDEF},
    {"typeid", KW_TYPEID},
    {"typename", KW_TYPENAME},
    {"union", KW_UNION},
    {"unsigned", KW_UNSIGNED},
    {"using", KW_USING},
    {"virtual", KW_VIRTUAL},
    {"void", KW_VOID},
    {"volatile", KW_VOLATILE},
    {"wchar_t", KW_WCHAR_T},
    {"while", KW_WHILE},

    // operators/punctuation
    {"{", OP_LBRACE},
    {"<%", OP_LBRACE},
    {"}", OP_RBRACE},
    {"%>", OP_RBRACE},
    {"[", OP_LSQUARE},
    {"<:", OP_LSQUARE},
    {"]", OP_RSQUARE},
    {":>", OP_RSQUARE},
    {"(", OP_LPAREN},
    {")", OP_RPAREN},
    {"|", OP_BOR},
    {"bitor", OP_BOR},
    {"^", OP_XOR},
    {"xor", OP_XOR},
    {"~", OP_COMPL},
    {"compl", OP_COMPL},
    {"&", OP_AMP},
    {"bitand", OP_AMP},
    {"!", OP_LNOT},
    {"not", OP_LNOT},
    {";", OP_SEMICOLON},
    {":", OP_COLON},
    {"...", OP_DOTS},
    {"?", OP_QMARK},
    {"::", OP_COLON2},
    {".", OP_DOT},
    {".*", OP_DOTSTAR},
    {"+", OP_PLUS},
    {"-", OP_MINUS},
    {"*", OP_STAR},
    {"/", OP_DIV},
    {"%", OP_MOD},
    {"=", OP_ASS},
    {"<", OP_LT},
    {">", OP_GT},
    {"+=", OP_PLUSASS},
    {"-=", OP_MINUSASS},
    {"*=", OP_STARASS},
    {"/=", OP_DIVASS},
    {"%=", OP_MODASS},
    {"^=", OP_XORASS},
    {"xor_eq", OP_XORASS},
    {"&=", OP_BANDASS},
    {"and_eq", OP_BANDASS},
    {"|=", OP_BORASS},
    {"or_eq", OP_BORASS},
    {"<<", OP_LSHIFT},
    {">>", OP_RSHIFT},
    {">>=", OP_RSHIFTASS},
    {"<<=", OP_LSHIFTASS},
    {"==", OP_EQ},
    {"!=", OP_NE},
    {"not_eq", OP_NE},
    {"<=", OP_LE},
    {">=", OP_GE},
    {"&&", OP_LAND},
    {"and", OP_LAND},
    {"||", OP_LOR},
    {"or", OP_LOR},
    {"++", OP_INC},
    {"--", OP_DEC},
    {",", OP_COMMA},
    {"->*", OP_ARROWSTAR},
    {"->", OP_ARROW}
};

// map of enum to string
const map<ETokenType, string> TokenTypeToStringMap =
{
    {KW_ALIGNAS, "KW_ALIGNAS"},
    {KW_ALIGNOF, "KW_ALIGNOF"},
    {KW_ASM, "KW_ASM"},
    {KW_AUTO, "KW_AUTO"},
    {KW_BOOL, "KW_BOOL"},
    {KW_BREAK, "KW_BREAK"},
    {KW_CASE, "KW_CASE"},
    {KW_CATCH, "KW_CATCH"},
    {KW_CHAR, "KW_CHAR"},
    {KW_CHAR16_T, "KW_CHAR16_T"},
    {KW_CHAR32_T, "KW_CHAR32_T"},
    {KW_CLASS, "KW_CLASS"},
    {KW_CONST, "KW_CONST"},
    {KW_CONSTEXPR, "KW_CONSTEXPR"},
    {KW_CONST_CAST, "KW_CONST_CAST"},
    {KW_CONTINUE, "KW_CONTINUE"},
    {KW_DECLTYPE, "KW_DECLTYPE"},
    {KW_DEFAULT, "KW_DEFAULT"},
    {KW_DELETE, "KW_DELETE"},
    {KW_DO, "KW_DO"},
    {KW_DOUBLE, "KW_DOUBLE"},
    {KW_DYNAMIC_CAST, "KW_DYNAMIC_CAST"},
    {KW_ELSE, "KW_ELSE"},
    {KW_ENUM, "KW_ENUM"},
    {KW_EXPLICIT, "KW_EXPLICIT"},
    {KW_EXPORT, "KW_EXPORT"},
    {KW_EXTERN, "KW_EXTERN"},
    {KW_FALSE, "KW_FALSE"},
    {KW_FLOAT, "KW_FLOAT"},
    {KW_FOR, "KW_FOR"},
    {KW_FRIEND, "KW_FRIEND"},
    {KW_GOTO, "KW_GOTO"},
    {KW_IF, "KW_IF"},
    {KW_INLINE, "KW_INLINE"},
    {KW_INT, "KW_INT"},
    {KW_LONG, "KW_LONG"},
    {KW_MUTABLE, "KW_MUTABLE"},
    {KW_NAMESPACE, "KW_NAMESPACE"},
    {KW_NEW, "KW_NEW"},
    {KW_NOEXCEPT, "KW_NOEXCEPT"},
    {KW_NULLPTR, "KW_NULLPTR"},
    {KW_OPERATOR, "KW_OPERATOR"},
    {KW_PRIVATE, "KW_PRIVATE"},
    {KW_PROTECTED, "KW_PROTECTED"},
    {KW_PUBLIC, "KW_PUBLIC"},
    {KW_REGISTER, "KW_REGISTER"},
    {KW_REINTERPET_CAST, "KW_REINTERPET_CAST"},
    {KW_RETURN, "KW_RETURN"},
    {KW_SHORT, "KW_SHORT"},
    {KW_SIGNED, "KW_SIGNED"},
    {KW_SIZEOF, "KW_SIZEOF"},
    {KW_STATIC, "KW_STATIC"},
    {KW_STATIC_ASSERT, "KW_STATIC_ASSERT"},
    {KW_STATIC_CAST, "KW_STATIC_CAST"},
    {KW_STRUCT, "KW_STRUCT"},
    {KW_SWITCH, "KW_SWITCH"},
    {KW_TEMPLATE, "KW_TEMPLATE"},
    {KW_THIS, "KW_THIS"},
    {KW_THREAD_LOCAL, "KW_THREAD_LOCAL"},
    {KW_THROW, "KW_THROW"},
    {KW_TRUE, "KW_TRUE"},
    {KW_TRY, "KW_TRY"},
    {KW_TYPEDEF, "KW_TYPEDEF"},
    {KW_TYPEID, "KW_TYPEID"},
    {KW_TYPENAME, "KW_TYPENAME"},
    {KW_UNION, "KW_UNION"},
    {KW_UNSIGNED, "KW_UNSIGNED"},
    {KW_USING, "KW_USING"},
    {KW_VIRTUAL, "KW_VIRTUAL"},
    {KW_VOID, "KW_VOID"},
    {KW_VOLATILE, "KW_VOLATILE"},
    {KW_WCHAR_T, "KW_WCHAR_T"},
    {KW_WHILE, "KW_WHILE"},
    {OP_LBRACE, "OP_LBRACE"},
    {OP_RBRACE, "OP_RBRACE"},
    {OP_LSQUARE, "OP_LSQUARE"},
    {OP_RSQUARE, "OP_RSQUARE"},
    {OP_LPAREN, "OP_LPAREN"},
    {OP_RPAREN, "OP_RPAREN"},
    {OP_BOR, "OP_BOR"},
    {OP_XOR, "OP_XOR"},
    {OP_COMPL, "OP_COMPL"},
    {OP_AMP, "OP_AMP"},
    {OP_LNOT, "OP_LNOT"},
    {OP_SEMICOLON, "OP_SEMICOLON"},
    {OP_COLON, "OP_COLON"},
    {OP_DOTS, "OP_DOTS"},
    {OP_QMARK, "OP_QMARK"},
    {OP_COLON2, "OP_COLON2"},
    {OP_DOT, "OP_DOT"},
    {OP_DOTSTAR, "OP_DOTSTAR"},
    {OP_PLUS, "OP_PLUS"},
    {OP_MINUS, "OP_MINUS"},
    {OP_STAR, "OP_STAR"},
    {OP_DIV, "OP_DIV"},
    {OP_MOD, "OP_MOD"},
    {OP_ASS, "OP_ASS"},
    {OP_LT, "OP_LT"},
    {OP_GT, "OP_GT"},
    {OP_PLUSASS, "OP_PLUSASS"},
    {OP_MINUSASS, "OP_MINUSASS"},
    {OP_STARASS, "OP_STARASS"},
    {OP_DIVASS, "OP_DIVASS"},
    {OP_MODASS, "OP_MODASS"},
    {OP_XORASS, "OP_XORASS"},
    {OP_BANDASS, "OP_BANDASS"},
    {OP_BORASS, "OP_BORASS"},
    {OP_LSHIFT, "OP_LSHIFT"},
    {OP_RSHIFT, "OP_RSHIFT"},
    {OP_RSHIFTASS, "OP_RSHIFTASS"},
    {OP_LSHIFTASS, "OP_LSHIFTASS"},
    {OP_EQ, "OP_EQ"},
    {OP_NE, "OP_NE"},
    {OP_LE, "OP_LE"},
    {OP_GE, "OP_GE"},
    {OP_LAND, "OP_LAND"},
    {OP_LOR, "OP_LOR"},
    {OP_INC, "OP_INC"},
    {OP_DEC, "OP_DEC"},
    {OP_COMMA, "OP_COMMA"},
    {OP_ARROWSTAR, "OP_ARROWSTAR"},
    {OP_ARROW, "OP_ARROW"}
};

// convert integer [0,15] to hexadecimal digit
char ValueToHexChar(int c)
{
    switch (c)
    {
    case 0: return '0';
    case 1: return '1';
    case 2: return '2';
    case 3: return '3';
    case 4: return '4';
    case 5: return '5';
    case 6: return '6';
    case 7: return '7';
    case 8: return '8';
    case 9: return '9';
    case 10: return 'A';
    case 11: return 'B';
    case 12: return 'C';
    case 13: return 'D';
    case 14: return 'E';
    case 15: return 'F';
    default: throw logic_error("ValueToHexChar of nonhex value");
    }
}

// hex dump memory range
string HexDump(const void* pdata, size_t nbytes)
{
    unsigned char* p = (unsigned char*) pdata;

    string s(nbytes*2, '?');

    for (size_t i = 0; i < nbytes; i++)
    {
        s[2*i+0] = ValueToHexChar((p[i] & 0xF0) >> 4);
        s[2*i+1] = ValueToHexChar((p[i] & 0x0F) >> 0);
    }

    return s;
}

// DebugPostTokenOutputStream: helper class to produce PA2 output format
struct DebugPostTokenOutputStream
{
    // output: invalid <source>
    void emit_invalid(const string& source)
    {
        cout << "invalid " << source << endl;
    }

    // output: simple <source> <token_type>
    void emit_simple(const string& source, ETokenType token_type)
    {
        cout << "simple " << source << " " << TokenTypeToStringMap.at(token_type) << endl;
    }

    // output: identifier <source>
    void emit_identifier(const string& source)
    {
        cout << "identifier " << source << endl;
    }

    // output: literal <source> <type> <hexdump(data,nbytes)>
    void emit_literal(const string& source, EFundamentalType type, const void* data, size_t nbytes)
    {
        cout << "literal " << source << " " << FundamentalTypeToStringMap.at(type) << " " << HexDump(data, nbytes) << endl;
    }

    // output: literal <source> array of <num_elements> <type> <hexdump(data,nbytes)>
    void emit_literal_array(const string& source, size_t num_elements, EFundamentalType type, const void* data, size_t nbytes)
    {
        cout << "literal " << source << " array of " << num_elements << " " << FundamentalTypeToStringMap.at(type) << " " << HexDump(data, nbytes) << endl;
    }

    // output: user-defined-literal <source> <ud_suffix> character <type> <hexdump(data,nbytes)>
    void emit_user_defined_literal_character(const string& source, const string& ud_suffix, EFundamentalType type, const void* data, size_t nbytes)
    {
        cout << "user-defined-literal " << source << " " << ud_suffix << " character " << FundamentalTypeToStringMap.at(type) << " " << HexDump(data, nbytes) << endl;
    }

    // output: user-defined-literal <source> <ud_suffix> string array of <num_elements> <type> <hexdump(data, nbytes)>
    void emit_user_defined_literal_string_array(const string& source, const string& ud_suffix, size_t num_elements, EFundamentalType type, const void* data, size_t nbytes)
    {
        cout << "user-defined-literal " << source << " " << ud_suffix << " string array of " << num_elements << " " << FundamentalTypeToStringMap.at(type) << " " << HexDump(data, nbytes) << endl;
    }

    // output: user-defined-literal <source> <ud_suffix> <prefix>
    void emit_user_defined_literal_integer(const string& source, const string& ud_suffix, const string& prefix)
    {
        cout << "user-defined-literal " << source << " " << ud_suffix << " integer " << prefix << endl;
    }

    // output: user-defined-literal <source> <ud_suffix> <prefix>
    void emit_user_defined_literal_floating(const string& source, const string& ud_suffix, const string& prefix)
    {
        cout << "user-defined-literal " << source << " " << ud_suffix << " floating " << prefix << endl;
    }

    // output : eof
    void emit_eof()
    {
        cout << "eof" << endl;
    }
};


// use these 3 functions to scan `floating-literals` (see PA2)
// for example PA2Decode_float("12.34") returns "12.34" as a `float` type
float PA2Decode_float(const string& s)
{
    istringstream iss(s);
    float x;
    iss >> x;
    return x;
}

double PA2Decode_double(const string& s)
{
    istringstream iss(s);
    double x;
    iss >> x;
    return x;
}

long double PA2Decode_long_double(const string& s)
{
    istringstream iss(s);
    long double x;
    iss >> x;
    return x;
}

unsigned int utf8Length(const string& s, unsigned int index = 0)
{
    unsigned int count = 0;

    for (unsigned int i = index; i < s.length(); i++)
    {
        unsigned char c = s[i];

        if (c < 0x80 || c > 0xbf)
            count++;
    }

    return count;
}

unsigned int utf8Decode(const string& s, unsigned long *result,
    unsigned int index = 0)
{
    unsigned int length = 0;
    unsigned int value = 0;

    // Verify the index is valid
    if (index >= s.length())
        return 0;

    // Get the length of the character based on the utf8 sequence
    if ((unsigned char)s[index] >= 0xf0 && (unsigned char)s[index] <= 0xf7)
    {
        length = 4;
        value = s[index] & 0x07;
    }
    else if ((unsigned char)s[index] >= 0xe0 && (unsigned char)s[index] <= 0xe8)
    {
        length = 3;
        value = s[index] & 0x0f;
    }
    else if ((unsigned char)s[index] >= 0xc0 && (unsigned char)s[index] <= 0xdf)
    {
        length = 2;
        value = s[index] & 0x1f;
    }
    else
    {
        length = 1;
        value = s[index];
    }

    // Verify the provided string is long enough, this is unlikely
    if (s.length() < length)
        throw runtime_error("invalid UTF8 sequence");

    for (unsigned int i = index+1; i < (index + length); i++)
    {
        value <<= 6;
        value |= s[i] & 0x3f;
    }

    *result = value;
    return length;
}

static string utf8Encode(const unsigned long c)
{
    string str;

    if (c < 0x7f)
        str.push_back((char)c);
    else if (c < 0x800)
    {
        str.push_back(0xc0 | (c >> 6));
        str.push_back(0x80 | (c & 0x3f));
    }
    else if (c < 0x10000)
    {
        str.push_back(0xe0 | (c >> 12));
        str.push_back(0x80 | ((c >> 6) & 0x3f));
        str.push_back(0x80 | (c & 0x3f));
    }
    else if (c < 0x110000)
    {
        str.push_back(0xf0 | (c >> 18));
        str.push_back(0x80 | ((c >> 12) & 0x3f));
        str.push_back(0x80 | ((c >> 6) & 0x3f));
        str.push_back(0x80 | (c & 0x3f));
    }

    return str;
}

static string utf16Encode(const unsigned long c)
{
    string str;
    uint16_t w1 = 0xD800;
    uint16_t w2 = 0xDC00;

    if (c < 0x10000)
        str.append((char*)&c, 2);
    else
    {
        w1 += (c - 0x10000) >> 10;
        w2 += (c - 0x10000) & 0x3ff;

        str.append((char*)&w1, 2);
        str.append((char*)&w2, 2);
    }

    return str;
}

static bool isUserSuffix(const string& str)
{
    // Most of the checking was done in the preprocessing stage, we simply need
    // to check that this is a valid identifier.  We can skip verifying ranges
    // from E.1 and E.2

    // Empty suffixes are not user defined
    if (str.length() == 0)
        return false;

    // The first character must be an underscore
    if (str[0] != '_')
        return false;


    for (char c : str)
    {
        if (!isalnum(c) && c != '_')
            return false;
    }

    return true;
}

class TokenStream : public IPPTokenStream
{
public:
    TokenStream(DebugPostTokenOutputStream& output)
        : mOutput(output)
    {}

    ~TokenStream() {}

    virtual void emit_whitespace_sequence()
    {
        // These are no longer needed
    }

    virtual void emit_new_line()
    {
        // These are no longer needed
    }

    virtual void emit_header_name(const string& data)
    {
        processStringLiterals();

        // These are no longer valid
        mOutput.emit_invalid(data);
    }

    virtual void emit_identifier(const string& data)
    {
        processStringLiterals();

        // If the identifier is in the token type map then emit it as a simple
        // token, otherwise it's an identifier
        if (StringToTokenTypeMap.find(data) != StringToTokenTypeMap.end())
            mOutput.emit_simple(data, StringToTokenTypeMap.at(data));
        else
            mOutput.emit_identifier(data);
    }

    EFundamentalType getFloatSuffixSize(const string& suffix)
    {
        if (suffix == "f" || suffix == "F")
            return FT_FLOAT;
        else if (suffix == "l" || suffix == "L")
            return FT_LONG_DOUBLE;
        else if (suffix == "")
            return FT_DOUBLE;

        return FT_INVALID;
    }

    void processFloat(const string& data)
    {

        string prefix;
        string suffix;
        char *tail = nullptr;
        long double value = 0;
        EFundamentalType type = FT_DOUBLE;

        // Convert the string to a value
        errno = 0;
        if ((value = strtold(data.c_str(), &tail)) == 0 && tail == data.c_str())
        {
            mOutput.emit_invalid(data);
            printError("invalid float: ", data);
            return;
        }

        // Check if the value was an overflow
        if (value == HUGE_VALL && errno == ERANGE)
        {
            mOutput.emit_invalid(data);
            printError("invalid float: ", data);
            return;
        }

        // Get the prefix and the suffix
        prefix = data.substr(0, tail - data.c_str());
        suffix = data.substr(tail - data.c_str());

        // Get the type based on the suffix
        if ((type = getFloatSuffixSize(suffix)) == FT_INVALID)
        {
            if (isUserSuffix(suffix))
            {
                // This must be a used defined suffix
                mOutput.emit_user_defined_literal_floating(data, suffix,
                    prefix);
                return;
            }
            else
            {
                mOutput.emit_invalid(data);
                printError("invalid float literal suffix: ", data);
                return;
            }
        }

        // Verify the returned value can fit into the specified type
        if ((type == FT_DOUBLE && value > DBL_MAX) ||
            (type == FT_FLOAT && value > FLT_MAX) ||
            (type == FT_LONG_DOUBLE && value > LDBL_MAX))
        {
            mOutput.emit_invalid(data);
            printError("float literal out of range: ", data);
            return;
        }

        // Output to match the test handler
        if (type == FT_DOUBLE)
        {
            double out = PA2Decode_double(data);
            mOutput.emit_literal(data, type, &out, sizeof(out));
        }
        else if (type == FT_FLOAT)
        {
            float out = PA2Decode_float(data);
            mOutput.emit_literal(data, type, &out, sizeof(out));
        }
        else
        {
            long double out = PA2Decode_long_double(data);
            mOutput.emit_literal(data, type, &out, sizeof(out));
        }
    }

    EFundamentalType sizeDecimal(unsigned long long value,
        EFundamentalType type)
    {
        switch (type)
        {
        case FT_INT:
            if (value <= INT_MAX) return FT_INT;
            /* Fall-through */
        case FT_LONG_INT:
            if (value <= LONG_MAX) return FT_LONG_INT;
            /* Fall-through */
        case FT_LONG_LONG_INT:
            if (value <= LLONG_MAX) return FT_LONG_LONG_INT;

            return FT_INVALID;

            break;
        case FT_UNSIGNED_INT:
            if (value <= UINT_MAX) return FT_UNSIGNED_INT;
            /* Fall-through */
        case FT_UNSIGNED_LONG_INT:
            if (value <= ULONG_MAX) return FT_UNSIGNED_LONG_INT;
            /* Fall-through */
        case FT_UNSIGNED_LONG_LONG_INT:
            if (value <= ULLONG_MAX) return FT_UNSIGNED_LONG_LONG_INT;

            return FT_INVALID;

            break;
        default:
            // This is not a valid integer type
            return FT_INVALID;
        }

        // This can't be reached
        return FT_INVALID;
    }

    EFundamentalType sizeOctOrHex(unsigned long long value,
        EFundamentalType type)
    {
        switch (type)
        {
        case FT_INT:
            if (value <= INT_MAX) return FT_INT;
            if (value <= UINT_MAX) return FT_UNSIGNED_INT;
            /* Fall-through */
        case FT_LONG_INT:
            if (value <= LONG_MAX) return FT_LONG_INT;
            if (value <= ULONG_MAX) return FT_UNSIGNED_LONG_INT;
            /* Fall-through */
        case FT_LONG_LONG_INT:
            if (value <= LLONG_MAX) return FT_LONG_LONG_INT;
            if (value <= ULLONG_MAX) return FT_UNSIGNED_LONG_LONG_INT;

            return FT_INVALID;

            break;
        case FT_UNSIGNED_INT:
            if (value <= UINT_MAX) return FT_UNSIGNED_INT;
            /* Fall-through */
        case FT_UNSIGNED_LONG_INT:
            if (value <= ULONG_MAX) return FT_UNSIGNED_LONG_INT;
            /* Fall-through */
        case FT_UNSIGNED_LONG_LONG_INT:
            if (value <= ULLONG_MAX) return FT_UNSIGNED_LONG_LONG_INT;

            return FT_INVALID;

            break;
        default:
            // This is not an integer type
            return FT_INVALID;
        }
    }

    EFundamentalType getIntSuffixSize(const string& suffix)
    {
        // Verify this is a valid suffix
        if (suffix == "")
            return FT_INT;

        if (suffix[0] == 'u' || suffix[0] == 'U')
        {
            if (suffix.length() == 1)
                return FT_UNSIGNED_INT;
            else if (suffix[1] == 'l' || suffix[1] == 'L')
            {
                if (suffix.length() == 2)
                    return FT_UNSIGNED_LONG_INT;
                else if (suffix[2] == suffix[1])
                    return FT_UNSIGNED_LONG_LONG_INT;
            }
            
            return FT_INVALID;
        }

        if (suffix[0] == 'l' || suffix[0] == 'L')
        {
            if (suffix.length() == 1)
                return FT_LONG_INT;
            else if (suffix[1] == suffix[0])
            {
                if (suffix.length() == 2)
                    return FT_LONG_LONG_INT;
                else if (suffix[2] == 'u' || suffix[2] == 'U')
                    return FT_UNSIGNED_LONG_LONG_INT;
            }
            else if (suffix[1] == 'u' || suffix[1] == 'U')
                return FT_UNSIGNED_LONG_INT;

            return FT_INVALID;
        }

        return FT_INVALID;
    }

    void processDecimal(const string& data)
    {
        unsigned int index = 0;
        unsigned long long value = 0;
        char *end = nullptr;
        string prefix;
        string suffix;
        EFundamentalType type = FT_INT;
        unsigned int size = 4;

        // Walk forward as long as we have digits
        while (index < data.length() && (data[index] >= '0' &&
                data[index] <= '9'))
            index++;
        prefix = data.substr(0, index);

        // Make sure we found some digits
        if (prefix.length() == 0)
        {
            mOutput.emit_invalid(data);
            printError("invalid decimal: ", data);
            return;
        }

        // Get any suffix
        suffix = data.substr(index);

        if (isUserSuffix(suffix))
        {
            // This must be a user defined suffix
            mOutput.emit_user_defined_literal_integer(data, suffix, prefix);
            return;
        }

        // Get the value of the integer.
        errno = 0;
        value = strtoull(prefix.c_str(), &end, 10);
        if (value == 0 && prefix.c_str() == end)
        {
            mOutput.emit_invalid(data);
            printError("unable to parse decimal: ", data);
            return;
        }

        // A return value of ULLONG_MAX could mean the value was too large to
        // fit into an unsigned long long
        if (value == ULLONG_MAX && errno != 0)
        {
            mOutput.emit_invalid(data);
            printError("decimal integer literal out of range: ", data);
            return;
        }

        // Get the suffix size
        if ((type = getIntSuffixSize(suffix)) == FT_INVALID &&
            suffix.length() > 0)
        {
            // This is invalid
            mOutput.emit_invalid(data);
            printError("Invalid integer literal suffix: ", data);
            return;
        }

        // Verify that the value will fit into a size of the same type
        if ((type = sizeDecimal(value, type)) == FT_INVALID)
        {
            mOutput.emit_invalid(data);
            printError("decimal integer literal out of range: ", data);
            return;
        }

        // Get the size of the output
        if (type == FT_LONG_INT || type == FT_UNSIGNED_LONG_INT ||
                type == FT_LONG_LONG_INT || type == FT_UNSIGNED_LONG_LONG_INT)
            size = 8;

        mOutput.emit_literal(data, type, &value, size);
    }

    void processHexidecimal(const string& data)
    {
        unsigned int index = 2;
        unsigned long long value;
        string prefix;
        string suffix;
        EFundamentalType type = FT_INT;
        unsigned int size = 4;

        // Consume as long as we find hexidecimal digits
        while (index < data.length() && ((data[index] >= '0' &&
                data[index] <= '9') || (data[index] >= 'a' &&
                data[index] <= 'f') || (data[index] >= 'A' &&
                data[index] <= 'F')))
            index++;
        prefix = data.substr(0, index);

        // Make sure we found some digits
        if (prefix.length() == 2)
        {
            mOutput.emit_invalid(data);
            printError("invalid hexidecimal: ", data);
            return;
        }

        // Get any suffix
        suffix = data.substr(index);

        if (isUserSuffix(suffix))
        {
            // This must be a user defined suffix
            mOutput.emit_user_defined_literal_integer(data, suffix, prefix);
            return;
        }

        // Get the value.  This can be zero
        errno = 0;
        value = strtoull(prefix.c_str(), nullptr, 16);

        // A value of ULLONG_MAX could mean that the input value was too large
        // to fit into an unsigned long long.
        if (value == ULLONG_MAX && errno != 0)
        {
            mOutput.emit_invalid(data);
            printError("hexidecimal literal out of range: ", data);
            return;
        }

        // Get the suffix size
        if ((type = getIntSuffixSize(suffix)) == FT_INVALID)
        {
            if (suffix.length() > 0)
            {
                // This is invalid
                mOutput.emit_invalid(data);
                printError("Invalid integer literal suffix: ", data);
                return;
            }
        }

        // Verify that the value will fit into a size of the same type
        if ((type = sizeOctOrHex(value, type)) == FT_INVALID)
        {
            mOutput.emit_invalid(data);
            printError("hexidecimal integer literal out of range: ", data);
            return;
        }

        // Get the size of the output
        if (type == FT_LONG_INT || type == FT_UNSIGNED_LONG_INT ||
                type == FT_LONG_LONG_INT || type == FT_UNSIGNED_LONG_LONG_INT)
            size = 8;

        mOutput.emit_literal(data, type, &value, size);
    }
    
    void processOctal(const string& data)
    {
        unsigned int index = 0;
        unsigned long long value = 0;
        string prefix;
        string suffix;
        EFundamentalType type = FT_INT;
        unsigned int size = 4;

        // Consume as long as we find octal digits
        while (index < data.length() && (data[index] >= '0' && 
                data[index] <= '7'))
            index++;
        prefix = data.substr(0, index);

        // Make sure we found some digits
        if (prefix.length() == 0)
        {
            mOutput.emit_invalid(data);
            printError("invalid octal: ", data);
            return;
        }

        // Get any suffix
        suffix = data.substr(index);

        if (isUserSuffix(suffix))
        {
            // This must be a user defined suffix
            mOutput.emit_user_defined_literal_integer(data, suffix, prefix);
            return;
        }

        // Get the value.  This can be zero.
        errno = 0;
        value = strtoull(prefix.c_str(), nullptr, 8);

        // We need to check that a value of ULLONG_MAX was specified by the
        // input, otherwise it means the value cannot fit in an unsigned long
        // long.
        if (value == ULLONG_MAX && errno == ERANGE)
        {
            mOutput.emit_invalid(data);
            printError("octal literal out of range: ", data);
            return;
        }

        // Get the suffix size
        if ((type = getIntSuffixSize(suffix)) == FT_INVALID)
        {
            if (suffix.length() > 0)
            {
                // This is invalid
                mOutput.emit_invalid(data);
                printError("Invalid integer literal suffix: ", data);
                return;
            }
        }

        // Verify that the value will fit into a size of the same type
        if ((type = sizeOctOrHex(value, type)) == FT_INVALID)
        {
            mOutput.emit_invalid(data);
            printError("octal integer literal out of range: ", data);
            return;
        }

        // Get the size of the output
        if (type == FT_LONG_INT || type == FT_UNSIGNED_LONG_INT ||
                type == FT_LONG_LONG_INT || type == FT_UNSIGNED_LONG_LONG_INT)
            size = 8;

        mOutput.emit_literal(data, type, &value, size);
    }

    virtual void emit_pp_number(const string& data)
    {
        processStringLiterals();

        if (data.length() > 2 && data[0] == '0' && \
                (data[1] == 'x' || data[1] == 'X'))
        {
            processHexidecimal(data);
            return;
        }

        if (data.find(".") != string::npos)
        {
            processFloat(data);
            return;
        }

        // For this to be an octal number it must start with a zero and be
        // followed by octal digits
        if (data.length() >= 1 && data[0] == '0')
        {
            bool valid = true;

            for (char c : data)
            {
                // This is still a valid octal if it has a suffix
                if (c == '_' || c == 'l' || c == 'L' || c == 'u' || c == 'U')
                    break;

                if (c < '0' || c > '7')
                {
                    valid = false;
                    break;
                }
            }

            if (valid)
            {
                processOctal(data);
                return;
            }
        }

        // Check if this is a float with an exponent
        string::size_type underscore = data.find("_");
        if (underscore != string::npos)
        {
            string::size_type exponent = data.find("e");

            if (exponent != string::npos && exponent < underscore)
            {
                processFloat(data);
                return;
            }
            else if ((exponent = data.find("E")) != string::npos &&
                    exponent < underscore)
            {
                processFloat(data);
                return;
            }
        }
        else if (data.find("e") != string::npos ||
            data.find("E") != string::npos)
        {
            processFloat(data);
            return;
        }

        // Try processing as a decimal
        processDecimal(data);
    }

    bool processCharLiteral(const string& data, EFundamentalType *type,
        unsigned long *value, unsigned int *size)
    {
        string str;

        *type = FT_CHAR;
        *size = 1;
        *value = 0;

        // Check for a prefix
        if (data[0] == 'u')
        {
            *type = FT_CHAR16_T;
            str = data.substr(1);
        }
        else if (data[0] == 'U')
        {
            *type = FT_CHAR32_T;
            str = data.substr(1);
        }
        else if (data[0] == 'L')
        {
            *type = FT_WCHAR_T;
            str = data.substr(1);
        }
        else
        {
            str = data;
        }

        // Strip the quotes
        str = str.substr(1, str.length()-2);

        // Verify the literal isn't empty
        if (str.length() == 0)
        {
            mOutput.emit_invalid(data);
            printError("malformed character literal: ", data);
            return false;
        }
        // A single character can't be an escape sequencce
        else if (str.length() == 1)
        {
            *value = str[0];
        }
        // Check if this is an escape sequence
        else if (str.length() > 1 && str[0] == '\\')
        {
            // Check if this is an escape sequence
            switch (str[1])
            {
            case 'n':
                *value = 0x0a;
                break;
            case 't':
                *value = 0x09;
                break;
            case 'v':
                *value = 0x0b;
                break;
            case 'b':
                *value = 0x08;
                break;
            case 'r':
                *value = 0x0d;
                break;
            case 'f':
                *value = 0x0c;
                break;
            case 'a':
                *value = 0x07;
                break;
            case '\\':
                *value = 0x5c;
                break;
            case '?':
                *value = 0x3f;
                break;
            case '\'':
                *value = 0x27;
                break;
            case '"':
                *value = 0x22;
                break;
            default:
                // Check if this is an octal number
                const char *start;
                char *end;
                unsigned int prefixLen;

                // Hexidecimal values are prefixed with an 'x'
                if (str[1] == 'x')
                {
                    prefixLen = 2;
                    start = str.c_str() + prefixLen;
                    errno = 0;
                    *value = strtoul(start, &end, 16);
                }
                else
                {
                    prefixLen = 1;
                    start = str.c_str() + prefixLen;
                    errno = 0;
                    *value = strtoul(start, &end, 8);
                }

                // Verify the value was parsed
                if (*value == ULONG_MAX && errno == ERANGE)
                {
                    mOutput.emit_invalid(data);
                    printError("escape value out of range: ", str);
                    return false;
                }

                // Verify that there are no extra characters
                if ((unsigned int)((end - start) + prefixLen) < str.length())
                {
                    mOutput.emit_invalid(data);
                    printError("multicharacter literals are not supported: ",
                        data);
                    return false;
                }

                break;
            }
        }
        else if (utf8Length(str) == 1)
        {
            utf8Decode(str, value);
        }
        else
        {
            mOutput.emit_invalid(data);
            printError("multicharacter literals are not supported: ", data);
            return false;
        }

        // Verify that the parsed value can fit into the specified type
        if (*type == FT_CHAR && *value > CHAR_MAX)
            *type = FT_INT;
        else if (*type == FT_CHAR16_T && *value > 0xffff)
        {
            mOutput.emit_invalid(data);
            printError("UTF-16 character literal out of range: ", data);
            return false;
        }

        // Verify the value is within the course defined ranges
        if ((*value >= 0xd800 && *value < 0xe000) || (*value >= 0x110000))
        {
            mOutput.emit_invalid(data);
            printError("character literal is not within course defined range: ",
                str);
            return false;
        }

        if (*type == FT_CHAR16_T)
            *size = 2;
        else if (*type == FT_CHAR32_T || *type == FT_WCHAR_T || *type == FT_INT)
            *size = 4;

        return true;
    }

    virtual void emit_character_literal(const string& data)
    {
        EFundamentalType type;
        unsigned long value;
        unsigned int size;

        processStringLiterals();

        // Process the literal
        if (!processCharLiteral(data, &type, &value, &size))
            return;

        mOutput.emit_literal(data, type, &value, size);
    }

    virtual void emit_user_defined_character_literal(const string& data)
    {
        EFundamentalType type;
        unsigned long value;
        unsigned int size;
        string::size_type endQuote = 0;
        string suffix;

        processStringLiterals();

        // Strip the suffix
        endQuote = data.rfind("'");
        suffix = data.substr(endQuote+1);

        // Verify the suffix is valid
        if (!isUserSuffix(suffix))
        {
            mOutput.emit_invalid(data);
            printError("invalid user defined suffix: ", suffix);
            return;
        }

        // Process the literal
        if (!processCharLiteral(data.substr(0, endQuote+1), &type, &value, &size))
            return;

        mOutput.emit_user_defined_literal_character(data, suffix, type, &value,
            size);
    }

    void invalidateStringLiterals(const string& err)
    {
        cout << "invalid";
        for (string s : mStrings)
        {
            cout << " " << s;
        }
        cout << endl;
        mStrings.clear();

        printError(err, "");
    }

    bool appendEncoded(string& output, unsigned long c, EFundamentalType type)
    {
        if (type == FT_CHAR)
            output.append(utf8Encode(c));
        else if (type == FT_CHAR16_T)
            output.append(utf16Encode(c));
        else if (type == FT_CHAR32_T || type == FT_WCHAR_T)
            output.append((char*)&c, 4);
        else
            // What is this??
            return false;

        return true;
    }

    void processStringLiterals()
    {
        string prefix, suffix, source, str;
        bool convert = false, isRaw = false;
        EFundamentalType type = FT_CHAR;
        unsigned int size = 1;

        // Don't do anything if there are no literals queued up
        if (mStrings.size() == 0)
            return;

        // Loop over the strings to find the code unit type
        for (string s : mStrings)
        {
            string::size_type quote = s.find("\"");

            // Check if there is a prefix
            if (quote == 0)
                continue;
            
            // Verify the prefixes match
            if (prefix.length() > 0)
            {
                if (s.substr(0, quote) != prefix)
                {
                    invalidateStringLiterals("mismatched encoding prefix"
                        " in string literal sequence");
                    return;
                }
            }
            else
            {
                prefix = s.substr(0, quote);
            }
        }

        // Check if this is a valid prefix
        if (prefix == "u8" || prefix == "u8R" || prefix == "R" || prefix == "")
        {
            type = FT_CHAR;
            size = 1;
        }
        else if (prefix == "u" || prefix == "uR")
        {
            type = FT_CHAR16_T;
            size = 2;
        }
        else if (prefix == "U" || prefix == "UR")
        {
            type = FT_CHAR32_T;
            size = 4;
        }
        else if (prefix == "L" || prefix == "LR")
        {
            type = FT_WCHAR_T;
            size = 4;
        }
        else
        {
            // This will likely never happen
            mOutput.emit_invalid(source);
            printError("invalid string literal prefix: ", prefix);
            return;
        }

        // Loop over the strings converting
        for (string s : mStrings)
        {
            string::size_type startData, endData;
            string::size_type startQuote = s.find("\"");
            string::size_type endQuote = s.rfind("\"");
            isRaw = false;

            // Append this string to the source
            if (source.length() == 0)
                source.append(s);
            else
            {
                source.push_back(' ');
                source.append(s);
            }

            // Check if there is a prefix
            if (startQuote > 0)
            {
                // Check if this is a raw string
                if (s.substr(0, startQuote).find("R") != string::npos)
                    isRaw = true;
            }

            // Check if there is a suffix
            if (endQuote < s.length()-1)
            {
                // Verify the suffixes match
                if (suffix.length() > 0)
                {
                    if (s.substr(endQuote+1) != suffix)
                    {
                        invalidateStringLiterals("mismatched ud_suffix in"
                            "string literal sequence");
                        return;
                    }
                }
                else
                {
                    suffix = s.substr(endQuote+1);

                    // Verify this is a valid suffix
                    if (!isUserSuffix(suffix))
                    {
                        invalidateStringLiterals("invalid user defined suffix");
                        return;
                    }
                }
            }

            if (isRaw)
            {
                startData = s.find("(", startQuote) + 1;
                endData = endQuote - (startData - startQuote) + 1;
            }
            else
            {
                startData = startQuote + 1;
                endData = endQuote;
            }

            for (unsigned int i = startData; i < endData; /* empty */)
            {
                unsigned long c;

                // Get the next character
                i += utf8Decode(s, &c, i); 

                // Check if this is an escape sequence
                if (convert)
                {
                    switch (c)
                    {
                    case 'a':
                        appendEncoded(str, 0x07, type);
                        break;
                    case 'b':
                        appendEncoded(str, 0x08, type);
                        break;
                    case 't':
                        appendEncoded(str, 0x09, type);
                        break;
                    case 'n':
                        appendEncoded(str, 0x0a, type);
                        break;
                    case 'v':
                        appendEncoded(str, 0x0b, type);
                        break;
                    case 'f':
                        appendEncoded(str, 0x0c, type);
                        break;
                    case 'r':
                        appendEncoded(str, 0x0d, type);
                        break;
                    case '\'':
                        appendEncoded(str, '\'', type);
                        break;
                    case '"':
                        appendEncoded(str, '"', type);
                        break;
                    case '?':
                        appendEncoded(str, '?', type);
                        break;
                    case '\\':
                        appendEncoded(str, '\\', type);
                        break;
                    case 'x':
                        {
                            const char *start = s.c_str() + i;
                            char *end = nullptr;
                            unsigned long value;

                            // Decode the value
                            errno = 0;
                            value = strtoul(start, &end, 16);

                            // This is already validated in preprocessing but
                            // we'll check here just to be thorough.
                            if (start == end)
                            {
                                invalidateStringLiterals("invalid hex escape "
                                    "sequence");
                                return;
                            }

                            // Verify the value isn't larger than an unsigned
                            // long and is in the valid course-defined range.
                            if ((value == ULONG_MAX && errno != 0) ||
                                (value >= 0xD800 && value < 0xE000) ||
                                (value >= 0x110000))
                            {
                                invalidateStringLiterals("hexidecimal value "
                                    "out-of-range");
                                return;
                            }

                            // Encode the value
                            appendEncoded(str, value, type);
                            i += end - (s.c_str() + i);
                        }
                        break;
                    default:
                        {
                            // If this is not a digit it must be a bad escape
                            // sequence
                            if (!isdigit(c))
                            {
                                invalidateStringLiterals("invalid escape "
                                    "sequence");
                            }

                            const char *start = s.c_str() + i - 1;
                            char *end = nullptr;
                            unsigned long value;

                            // Decode the value
                            errno = 0;
                            value = strtoul(start, &end, 8);

                            // This is already validated in preprocessing but
                            // we'll check here just to be thorough.
                            if (start == end)
                            {
                                invalidateStringLiterals("invalid octal escape "
                                    "sequence");
                                return;
                            }

                            // An octal literal can only be three digits so it
                            // can't overflow an unsigned long so just encode
                            // it
                            appendEncoded(str, value, type);
                            i += end - (s.c_str() + i);
                        }
                    }

                    convert = false;
                }
                else if (!isRaw && c == '\\')
                    convert = true;
                else
                    appendEncoded(str, c, type);
            }
        }

        // Append the terminating character
        appendEncoded(str, 0, type);

        // If there is a suffix, then this is user defined
        if (suffix.length() > 0)
            mOutput.emit_user_defined_literal_string_array(source, suffix,
                str.length() / size, type, str.c_str(), str.length());
        else
            mOutput.emit_literal_array(source, str.length() / size, type,
                str.c_str(), str.length());

        mStrings.clear();
    }

    virtual void emit_string_literal(const string& data)
    {
        // Add this string to the list of strings
        mStrings.push_back(data);
    }

    virtual void emit_user_defined_string_literal(const string& data)
    {
        // Add this string to the list of strings
        mStrings.push_back(data);
    }

    virtual void emit_preprocessing_op_or_punc(const string& data)
    {
        processStringLiterals();

        // At this point preprocessing identifiers are invalid
        if (data == "#" || data == "##" || data == "%:" || data == "%:%:")
            mOutput.emit_invalid(data);
        // Check if this is a simple token
        else if (StringToTokenTypeMap.find(data) != StringToTokenTypeMap.end())
            mOutput.emit_simple(data, StringToTokenTypeMap.at(data));
        else
        {
            mOutput.emit_invalid(data);
            printError("Invalid preprocessing operator: ", data);
        }
    }

    virtual void emit_non_whitespace_char(const string& data)
    {
        processStringLiterals();

        // This is an invalid token
        mOutput.emit_invalid(data);
        printError("Non-whitespace characters are invalid: ", data);
    }

    virtual void emit_eof()
    {
        processStringLiterals();

        mOutput.emit_eof();
    }

    void printError(const string& msg, const string& value)
    {
        cerr << "ERROR: " << msg << value << endl;
    }

protected:
    DebugPostTokenOutputStream& mOutput;
    vector<string> mStrings;
};

int main()
{
    // TODO:
    // 1. apply your code from PA1 to produce `preprocessing-tokens`
    // 2. "post-tokenize" the `preprocessing-tokens` as described in PA2
    // 3. write them out in the PA2 output format specifed

    // You may optionally use the above starter code.
    //
    // In particular there is the DebugPostTokenOutputStream class which helps form the
    // correct output format:

    DebugPostTokenOutputStream output;
    TokenStream stream(output);
    PPTokenizer tokenizer(stream);

    try
    {
        ostringstream oss;
        oss << cin.rdbuf();

        string input = oss.str();

        for (char c : input)
        {
            unsigned char code_unit = c;
            tokenizer.process(code_unit);
        }

        tokenizer.process(EndOfFile);
    }
    catch (exception& e)
    {
        cerr << "ERROR: " << e.what() << endl;
        return EXIT_FAILURE;
    }

    // output.emit_invalid("foo");
    // output.emit_simple("auto", KW_AUTO);

    // u16string bar = u"bar";
    // output.emit_literal_array("u\"bar\"", bar.size()+1, FT_CHAR16_T, bar.data(), bar.size() * 2 + 2);

    // output.emit_user_defined_literal_integer("123_ud1", "ud1", "123");
}
