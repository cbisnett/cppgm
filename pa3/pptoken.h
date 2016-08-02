/// Definitions for the PPTokenizer class
///
/// @file pptoken.h
/// @author Chris Bisnett
/// @date 03.20.13

#pragma once

#include "IPPTokenStream.h"

// EndOfFile: synthetic "character" to represent the end of source file
constexpr int EndOfFile = -1;

// Tokenizer
class PPTokenizer
{
public:
    PPTokenizer(IPPTokenStream& output);

    void process(int c);

protected:
    enum TransState {
        TRANS_START = 0,
        TRIGRAPH_DECODE,
        TRIGRAPH_DECODE_2,
        UCN_OR_LINE_SPLICE,
        UCN_DECODE_16,
        UCN_DECODE_32,
    };

    enum State {
        PTOKEN_START=1,
        INCLUDE_HASH,
        INCLUDE_KEYWORD,
        INCLUDE_WS,
        HEADER_NAME_H,
        HEADER_NAME_Q,
        IDENTIFIER,
        PP_NUMBER,
        PP_NUMBER_EXP,
        PP_NUMBER_FRAC,
        CHAR_LITERAL,
        CHAR_LITERAL_MAYBE_USER,
        USER_CHAR_LITERAL,
        STRING_LITERAL,
        STRING_LITERAL_MAYBE_USER,
        USER_STRING_LITERAL,
        ENCODING_PREFIX,
        RAW_STRING_LITERAL,
        RAW_STRING_LITERAL_START,
        RAW_STRING_LITERAL_DELIM,
        ESC_SEQUENCE,
        ESC_SEQUENCE_HEX,
        ESC_SEQUENCE_UCN_4,
        ESC_SEQUENCE_UCN_3,
        ESC_SEQUENCE_UCN_2,
        ESC_SEQUENCE_UCN_1,
        PRE_OP_OR_PUNC,
        PRE_OP_OR_PUNC_HASH,
        PRE_OP_OR_PUNC_COLON,
        PRE_OP_OR_PUNC_LT,
        PRE_OP_OR_PUNC_LT_LT,
        PRE_OP_OR_PUNC_LT_COLON,
        PRE_OP_OR_PUNC_LT_COLON_COLON,
        PRE_OP_OR_PUNC_GT,
        PRE_OP_OR_PUNC_GT_GT,
        PRE_OP_OR_PUNC_PERCENT,
        PRE_OP_OR_PUNC_PERCENT_COLON,
        PRE_OP_OR_PUNC_PERCENT_COLON_PERCENT,
        PRE_OP_OR_PUNC_DOT,
        PRE_OP_OR_PUNC_DOT_DOT,
        PRE_OP_OR_PUNC_PLUS,
        PRE_OP_OR_PUNC_MINUS,
        PRE_OP_OR_PUNC_MINUS_LT,
        PRE_OP_OR_PUNC_STAR,
        PRE_OP_OR_PUNC_SLASH,
        PRE_OP_OR_PUNC_PIPE,
        PRE_OP_OR_PUNC_CAROT,
        PRE_OP_OR_PUNC_BANG,
        PRE_OP_OR_PUNC_AMP,
        PRE_OP_OR_PUNC_EQUALS,
        COMMENT,
        COMMENT_ONELINE,
        COMMENT_MULTILINE,
        COMMENT_MULTILINE_2,
        WHITESPACE_SEQ,
        NEW_LINE,
    };

    bool translate(int c);

    IPPTokenStream& output;
    u32string mCpStream;
    unsigned int mForward;
    bool mTranslate;
    int mTransState;
    int mState;
    int mLastToken;
    int mReturnState;
    u32string mRawDelim;
};
