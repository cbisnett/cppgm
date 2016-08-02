// Interface for posttoken

#pragma once

#include <cstddef>
#include <map>
#include <vector>
#include <string>

#include "token.h"
#include "IPostTokenStream.h"

using namespace std;

class TokenStream : public IPostTokenStream
{
public:
    TokenStream(IPostTokenStream& output);
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
    IPostTokenStream& mOutput;
    vector<string> mStrings;
};
