#pragma once

#include <vector>
#include <string>

#include "token.h"
#include "pp.h"

using namespace std;

// DebugPostTokenOutputStream: helper class to produce PA2 output format
class IPostTokenOutputStream
{
public:
    virtual void emit_invalid(const string& source) = 0;
    virtual void emit_simple(const string& source, ETokenType token_type) = 0;
    virtual void emit_identifier(const string& source) = 0;
    virtual void emit_literal(const string& source, EFundamentalType type, const void* data, size_t nbytes) = 0;
    virtual void emit_literal_array(const string& source, size_t num_elements, EFundamentalType type, const void* data, size_t nbytes) = 0;
    virtual void emit_user_defined_literal_character(const string& source, const string& ud_suffix, EFundamentalType type, const void* data, size_t nbytes) = 0;
    virtual void emit_user_defined_literal_string_array(const string& source, const string& ud_suffix, size_t num_elements, EFundamentalType type, const void* data, size_t nbytes) = 0;
    virtual void emit_user_defined_literal_integer(const string& source, const string& ud_suffix, const string& prefix) = 0;
    virtual void emit_user_defined_literal_floating(const string& source, const string& ud_suffix, const string& prefix) = 0;
    virtual void emit_eof() = 0;
};

class TokenStream : public IPPTokenStream
{
public:
    TokenStream(IPostTokenOutputStream& output);
    ~TokenStream();

    virtual void emit_whitespace_sequence();
    virtual void emit_new_line();
    virtual void emit_header_name(const string& data);
    virtual void emit_identifier(const string& data);
    EFundamentalType getFloatSuffixSize(const string& suffix);
    void processFloat(const string& data);
    EFundamentalType sizeDecimal(unsigned long long value,
        EFundamentalType type);
    EFundamentalType sizeOctOrHex(unsigned long long value,
        EFundamentalType type);
    EFundamentalType getIntSuffixSize(const string& suffix);
    void processDecimal(const string& data);
    void processHexidecimal(const string& data);
    void processOctal(const string& data);
    virtual void emit_pp_number(const string& data);
    bool processCharLiteral(const string& data, EFundamentalType *type,
        unsigned long *value, unsigned int *size);
    virtual void emit_character_literal(const string& data);
    virtual void emit_user_defined_character_literal(const string& data);
    void invalidateStringLiterals(const string& err);
    bool appendEncoded(string& output, unsigned long c, EFundamentalType type);
    void processStringLiterals();
    virtual void emit_string_literal(const string& data);
    virtual void emit_user_defined_string_literal(const string& data);
    virtual void emit_preprocessing_op_or_punc(const string& data);
    virtual void emit_non_whitespace_char(const string& data);
    virtual void emit_eof();
    void printError(const string& msg, const string& value);

protected:
    IPostTokenOutputStream& mOutput;
    vector<string> mStrings;
};
