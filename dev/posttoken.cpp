#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <stdexcept>

#include "pp.h"
#include "post.h"

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

class DebugPostTokenOutputStream : public IPostTokenOutputStream
{
public:
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