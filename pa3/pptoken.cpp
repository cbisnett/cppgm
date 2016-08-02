#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

using namespace std;

#include "pptoken.h"

// given hex digit character c, return its value
static int HexCharToValue(int c)
{
    switch (c)
    {
    case '0': return 0;
    case '1': return 1;
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 5;
    case '6': return 6;
    case '7': return 7;
    case '8': return 8;
    case '9': return 9;
    case 'A': return 10;
    case 'a': return 10;
    case 'B': return 11;
    case 'b': return 11;
    case 'C': return 12;
    case 'c': return 12;
    case 'D': return 13;
    case 'd': return 13;
    case 'E': return 14;
    case 'e': return 14;
    case 'F': return 15;
    case 'f': return 15;
    default: throw logic_error("HexCharToValue of nonhex char");
    }
}

// See C++ standard 2.11 Identifiers and Appendix/Annex E.1
static const vector<pair<int, int>> AnnexE1_Allowed_RangesSorted =
{
    {0xA8,0xA8},
    {0xAA,0xAA},
    {0xAD,0xAD},
    {0xAF,0xAF},
    {0xB2,0xB5},
    {0xB7,0xBA},
    {0xBC,0xBE},
    {0xC0,0xD6},
    {0xD8,0xF6},
    {0xF8,0xFF},
    {0x100,0x167F},
    {0x1681,0x180D},
    {0x180F,0x1FFF},
    {0x200B,0x200D},
    {0x202A,0x202E},
    {0x203F,0x2040},
    {0x2054,0x2054},
    {0x2060,0x206F},
    {0x2070,0x218F},
    {0x2460,0x24FF},
    {0x2776,0x2793},
    {0x2C00,0x2DFF},
    {0x2E80,0x2FFF},
    {0x3004,0x3007},
    {0x3021,0x302F},
    {0x3031,0x303F},
    {0x3040,0xD7FF},
    {0xF900,0xFD3D},
    {0xFD40,0xFDCF},
    {0xFDF0,0xFE44},
    {0xFE47,0xFFFD},
    {0x10000,0x1FFFD},
    {0x20000,0x2FFFD},
    {0x30000,0x3FFFD},
    {0x40000,0x4FFFD},
    {0x50000,0x5FFFD},
    {0x60000,0x6FFFD},
    {0x70000,0x7FFFD},
    {0x80000,0x8FFFD},
    {0x90000,0x9FFFD},
    {0xA0000,0xAFFFD},
    {0xB0000,0xBFFFD},
    {0xC0000,0xCFFFD},
    {0xD0000,0xDFFFD},
    {0xE0000,0xEFFFD}
};

// See C++ standard 2.11 Identifiers and Appendix/Annex E.2
static const vector<pair<int, int>> AnnexE2_DisallowedInitially_RangesSorted =
{
    {0x300,0x36F},
    {0x1DC0,0x1DFF},
    {0x20D0,0x20FF},
    {0xFE20,0xFE2F}
};

// See C++ standard 2.13 Operators and punctuators
static const unordered_set<u32string> Digraph_IdentifierLike_Operators =
{
    U"new", U"delete", U"and", U"and_eq", U"bitand",
    U"bitor", U"compl", U"not", U"not_eq", U"or",
    U"or_eq", U"xor", U"xor_eq"
};

// See `simple-escape-sequence` grammar
static const unordered_set<int> SimpleEscapeSequence_CodePoints =
{
    '\'', '"', '?', '\\', 'a', 'b', 'f', 'n', 'r', 't', 'v'
};

#define IS_DIGIT(x) (x >= '0' && x <= '9')
#define IS_LETTER(x) ((x >= 'a' && x <= 'z') || (x >= 'A' && x <= 'Z'))
#define IS_HEXDIGIT(x) (IS_DIGIT(x) || (x >= 'a' && x <= 'f') || (x >= 'A' && x <= 'F'))
#define IS_IDNONDIGIT(x) (IS_LETTER(x) || x == '_')
#define IS_BASECHAR(x) ((x >= 0x09 && x <= 0x0c) || (x >= 0x20 && x <= 23) || (x >= 0x25 && x <= 0x3f) || (x >= 0x41 && x <= 0x5f) || (x >= 0x61 && x <= 0x7e))

#define NEXT_STATE(x) do { \
    mForward++; \
    mState = x;} while (false)

#define SET_STATE(x) mState = x

#define BACK_STATE(x, y) do { \
    mForward = y; \
    mState = x;} while (false)

#define EMIT_TOKEN(type, x) do { \
    output.emit_##type(utf8Encode(mCpStream.substr(0, x))); \
    RESET_STATE(x);} while (false)

#define RESET_STATE(x) do { \
    mCpStream.erase(0, x); \
    mForward = 0; \
    mLastToken = mState; \
    mState = PTOKEN_START;} while (false)

#define CALL_STATE(x) do { \
    mForward++; \
    mReturnState = mState; \
    mState = x;} while (false)

#define RETURN_STATE() do {\
    mState = mReturnState; \
    mReturnState = 0;} while (false)

PPTokenizer::PPTokenizer(IPPTokenStream& output)
:   output(output),
    mForward(0),
    mTranslate(true),
    mTransState(TRIGRAPH_DECODE),
    mState(PTOKEN_START),
    mLastToken(0),
    mReturnState(0)
{}

static bool isAnnexE1(int cp)
{
    for(vector<pair<int, int>>::const_iterator it = \
        AnnexE1_Allowed_RangesSorted.cbegin(); \
        it != AnnexE1_Allowed_RangesSorted.cend(); ++it)
    {
        if ((int)cp >= it->first && (int)cp <= it->second)
            return true;
    }

    return false;
}

static bool isAnnexE2(int cp)
{
    for(vector<pair<int, int>>::const_iterator it = \
        AnnexE2_DisallowedInitially_RangesSorted.cbegin(); \
        it != AnnexE2_DisallowedInitially_RangesSorted.cend(); ++it)
    {
        if ((int)cp >= it->first && (int)cp <= it->second)
            return true;
    }

    return false;
}

static int utf8Decode(int c)
{
    static int utf8Count = 0, utf8Value = 0;

    // Check if the first byte is valid
    if (utf8Count == 0)
    {
        if (c < 0x7f)
            return c;
        else if (c >= 0xf0 && c <= 0xf7)
        {
            utf8Count = 3;
            utf8Value = c & 0x07;
        }
        else if (c >= 0xe0 && c <= 0xe8)
        {
            utf8Count = 2;
            utf8Value = c & 0x0f;
        }
        else if (c >= 0xc0 && c <= 0xdf)
        {
            utf8Count = 1;
            utf8Value = c & 0x1f;
        }
        else
            throw runtime_error("invalid UTF8 sequence");

        return -1;
    }

    // Check that continuation bytes are valid
    if (c < 0x80 || c > 0xbf)
        throw runtime_error("invalid UTF8 sequence");

    utf8Value <<= 6;
    utf8Value |= c & 0x3f;
    utf8Count--;

    if (utf8Count == 0)
        return utf8Value;

    return -1;
}

static string utf8Encode(const u32string &input)
{
    string str;

    for (char32_t c : input)
    {
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
    }

    return str;
}

static int ucnDecode(u32string str)
{
    int out = 0;

    str.erase(0, 2);
    for (char c : str)
    {
        out <<= 4;
        out |= HexCharToValue(c);
    }

    return out;
}

bool PPTokenizer::translate(int c)
{
    static unsigned int state = 0, forward = 0;
    static u32string buffer;

    // Just exit if we reach the end of the file
    if (c == EndOfFile)
    {
        mCpStream.push_back(c);
        return true;
    }

    // Decode utf8
    if ((c = utf8Decode(c)) == -1)
        return false;

    // Check if we are not translating
    if (!mTranslate)
    {
        // @todo There is a bug somewhere here that has to do with raw
        // strings.  Remove the next two lines and try 200-trigraphs.t
        mCpStream.append(buffer);
        buffer.clear();
        mCpStream.push_back(c);
        return true;
    }

    buffer.push_back(c);
    while (forward < buffer.length())
    {
        unsigned int cp = buffer[forward];

        switch (state)
        {
        case TRANS_START:
            // This could be a question mark or a backslash
            if (cp == '?')
            {
                forward++;
                state = TRIGRAPH_DECODE;
            }
            else if (cp == '\\')
            {
                forward++;
                state = UCN_OR_LINE_SPLICE;
            }
            else
            {
                mCpStream.push_back(buffer[0]);
                buffer.erase(0, 1);

                forward = 0;
                state = TRANS_START;
                return true;
            }

            break;

        case TRIGRAPH_DECODE:
            // Check for the second question mark
            if (cp == '?')
            {
                forward++;
                state = TRIGRAPH_DECODE_2;
            }
            else
            {
                // The first character should be returned
                mCpStream.push_back(buffer[0]);
                buffer.erase(0, 1);

                forward = 0;
                state = TRANS_START;
                return true;
            }

            break;

        case TRIGRAPH_DECODE_2:
            // Check for the last character in a trigraph sequence.
            switch (cp)
            {
            case '=':
                buffer.clear();
                buffer.push_back('#');
                break;
            case '/':
                buffer.clear();
                buffer.push_back('\\');
                break;
            case '\'':
                buffer.clear();
                buffer.push_back('^');
                break;
            case '(':
                buffer.clear();
                buffer.push_back('[');
                break;
            case ')':
                buffer.clear();
                buffer.push_back(']');
                break;
            case '!':
                buffer.clear();
                buffer.push_back('|');
                break;
            case '<':
                buffer.clear();
                buffer.push_back('{');
                break;
            case '>':
                buffer.clear();
                buffer.push_back('}');
                break;
            case '-':
                buffer.clear();
                buffer.push_back('~');
                break;
            case '?':
                // This makes the third question mark.  Return the first and
                // stay in this state.
                mCpStream.push_back(buffer[0]);
                buffer.erase(0, 1);

                forward = 2;
                return true;
            default:
                // This is not a trigraph
                mCpStream.append(buffer.substr(0, 2));
                buffer.erase(0, 2);
            }

            forward = 0;
            state = TRANS_START;
            return true;

            break;

        case UCN_OR_LINE_SPLICE:
            // This is a universal-character-name if the next character is
            // a 'u' other wise it could be a line-splice
            if (cp == 'u')
            {
                forward++;
                state = UCN_DECODE_16;
            }
            else if (cp == 'U')
            {
                forward++;
                state = UCN_DECODE_32;
            }
            else if (cp == '\n')
            {
                buffer.erase(0, 2);

                forward = 0;
                state = TRANS_START;
            }
            else
            {
                mCpStream.append(buffer.substr(0, 2));
                buffer.erase(0, 2);

                forward = 0;
                state = TRANS_START;
                return true;
            }

            break;

        case UCN_DECODE_16:
            // For this to be a valid universal-character-name it must be
            // followed by hex digits
            if (IS_HEXDIGIT(cp))
                forward++;
            else
            {
                // Return what we parsed thus far
                mCpStream.append(buffer.substr(0, forward));
                buffer.erase(0, forward);

                forward = 0;
                state = TRANS_START;
                return true;
            }

            // Check if we have a full universal-character-code
            if (buffer.length() == 6)
            {
                mCpStream.push_back(ucnDecode(buffer));
                buffer.erase(0, 6);

                forward = 0;
                state = TRANS_START;
                return true;
            }

            break;

        case UCN_DECODE_32:
            // For this to be a valid universal-character-name it must be
            // followed by hex digits
            if (IS_HEXDIGIT(cp))
                forward++;
            else
            {
                // Return what we parsed thus far
                mCpStream.append(buffer.substr(0, forward));
                buffer.erase(0, forward);

                forward = 0;
                state = TRANS_START;
                return true;
            }

            // Check if we have a full universal-character-code
            if (buffer.length() == 10)
            {
                mCpStream.push_back(ucnDecode(buffer));
                buffer.erase(0, 10);

                forward = 0;
                state = TRANS_START;
                return true;
            }

            break;
        }
    }

    return false;
}

void PPTokenizer::process(int c)
{
    // Translate the read character.  When translate returns true we can
    // process whats available in the stream
    if (!translate(c))
        return;

    while (mForward < mCpStream.length())
    {
        char32_t cp = mCpStream[mForward];

        switch (mState)
        {
        case PTOKEN_START:
            // Check for pp-number
            if (IS_DIGIT(cp))
                NEXT_STATE(PP_NUMBER);
            // Check for character literal
            else if (cp == '\'')
                NEXT_STATE(CHAR_LITERAL);
            // Check for string literal
            else if (cp == '"')
                NEXT_STATE(STRING_LITERAL);
            // Check for string or character literals with special sizes
            else if (cp == 'u' || cp == 'U' || cp == 'L')
                NEXT_STATE(ENCODING_PREFIX);
            // Check for a raw string literal
            else if (cp == 'R')
                NEXT_STATE(RAW_STRING_LITERAL_START);
            // Check for comments
            else if (cp == '/')
                NEXT_STATE(COMMENT);
            // Check for an include header
            else if ((mLastToken == 0 || mLastToken == NEW_LINE) && \
                    cp == '#')
                NEXT_STATE(INCLUDE_HASH);
            // Check for a single preprocessing_op_or_punc character
            else if (cp == '{' || cp == '}' || cp == '[' || cp == ']' ||
                    cp == '(' || cp == ')' || cp == ';' || cp == '?' ||
                    cp == '~' || cp == ',')
                NEXT_STATE(PRE_OP_OR_PUNC);
            // Check for multiple preprocessing_op_or_punc characters
            else if (cp == '#') NEXT_STATE(PRE_OP_OR_PUNC_HASH);
            else if (cp == ':') NEXT_STATE(PRE_OP_OR_PUNC_COLON);
            else if (cp == '<') NEXT_STATE(PRE_OP_OR_PUNC_LT);
            else if (cp == '>') NEXT_STATE(PRE_OP_OR_PUNC_GT);
            else if (cp == '%') NEXT_STATE(PRE_OP_OR_PUNC_PERCENT);
            else if (cp == '.') NEXT_STATE(PRE_OP_OR_PUNC_DOT);
            else if (cp == '+') NEXT_STATE(PRE_OP_OR_PUNC_PLUS);
            else if (cp == '-') NEXT_STATE(PRE_OP_OR_PUNC_MINUS);
            else if (cp == '*') NEXT_STATE(PRE_OP_OR_PUNC_STAR);
            else if (cp == '/') NEXT_STATE(PRE_OP_OR_PUNC_SLASH);
            else if (cp == '|') NEXT_STATE(PRE_OP_OR_PUNC_PIPE);
            else if (cp == '^') NEXT_STATE(PRE_OP_OR_PUNC_CAROT);
            else if (cp == '!') NEXT_STATE(PRE_OP_OR_PUNC_BANG);
            else if (cp == '&') NEXT_STATE(PRE_OP_OR_PUNC_AMP);
            else if (cp == '=') NEXT_STATE(PRE_OP_OR_PUNC_EQUALS);
            // Check for whitespace
            else if (cp == ' ' || cp == '\t' || cp == '\v')
                NEXT_STATE(WHITESPACE_SEQ);
            // Check for new-line
            else if (cp == '\n')
                NEXT_STATE(NEW_LINE);
            // Handle the end of the file
            else if ((int)cp == EndOfFile)
            {
                // A new-line should be emitted at the end of a file unless
                // the file is empty
                if (mLastToken != 0 && mLastToken != NEW_LINE)
                    output.emit_new_line();

                output.emit_eof();
                return;
            }
            // Check for nondigit identifier
            else if (IS_IDNONDIGIT(cp))
                NEXT_STATE(IDENTIFIER);
            // Check for universal-character-name identifier
            else
            {
                if (isAnnexE1(cp) && !isAnnexE2(cp))
                    NEXT_STATE(IDENTIFIER);
                else
                    // Must be non-whitespace
                    EMIT_TOKEN(non_whitespace_char, mForward+1);
            }

            break;

        case INCLUDE_HASH:
            // This must be followed immediatly by the include token
            if (mCpStream.compare(0, mForward+1, U"#include", mForward+1) \
                    != 0)
                BACK_STATE(PRE_OP_OR_PUNC_HASH, 1);
            else
            {
                // Check if the whole string matched
                if (mCpStream.length() == 8)
                    NEXT_STATE(INCLUDE_KEYWORD);
                else
                    mForward++;
            }

            break;

        case INCLUDE_KEYWORD:
            // A whitespace character must follow the token for this to be
            // a valid include
            if (cp == ' ')
                NEXT_STATE(INCLUDE_WS);
            else
                BACK_STATE(PRE_OP_OR_PUNC_HASH, 1);

            break;

        case INCLUDE_WS:
            // A header name must start with a quotation or a less-than
            if (cp == '"')
                NEXT_STATE(HEADER_NAME_Q);
            else if (cp == '<')
                NEXT_STATE(HEADER_NAME_H);
            else
                BACK_STATE(PRE_OP_OR_PUNC_HASH, 1);

            break;

        case HEADER_NAME_Q:
            // Continue processing until we reach a closing character or a
            // new-line
            if (cp == '"')
            {
                EMIT_TOKEN(preprocessing_op_or_punc, 1);
                EMIT_TOKEN(identifier, 7);
                output.emit_whitespace_sequence();
                RESET_STATE(1);
                EMIT_TOKEN(header_name, mCpStream.length());
            }
            else if (cp == '\n')
                throw runtime_error("unterminated header name");
            else
                mForward++;

            break;

        case HEADER_NAME_H:
            // Continue consuming until we reach a closing character or a
            // new-line
            if (cp == '>')
            {
                EMIT_TOKEN(preprocessing_op_or_punc, 1);
                EMIT_TOKEN(identifier, 7);
                output.emit_whitespace_sequence();
                RESET_STATE(1);
                EMIT_TOKEN(header_name, mCpStream.length());
            }
            else if (cp == '\n')
                throw runtime_error("unterminated header name");
            else
                mForward++;

            break;

        case IDENTIFIER:
            // Continue comsuming an identifier until we reach something
            // other than a digit, non-digit, underscore, or special
            if (IS_DIGIT(cp) || IS_IDNONDIGIT(cp) || isAnnexE1(cp))
                mForward++;
            else
            {
                // Check if this is a digraph
                if (Digraph_IdentifierLike_Operators.find(mCpStream.substr(0, mForward)) != Digraph_IdentifierLike_Operators.end())
                    EMIT_TOKEN(preprocessing_op_or_punc, mForward);
                else
                    EMIT_TOKEN(identifier, mForward);
            }

            break;

        case PP_NUMBER:
            // The only way we can get here is if a token starts with a
            // digit or a decimal.  Valid values are digits, decimal,
            // identifier-nondigits, and exponents.
            // Exponents must be followed by a sign
            if (cp == 'e' || cp == 'E')
                NEXT_STATE(PP_NUMBER_EXP);
            else if (IS_DIGIT(cp) || IS_IDNONDIGIT(cp) || cp == '.')
                // Consume
                mForward++;
            else
                // This is the end of this token
                EMIT_TOKEN(pp_number, mForward);

            break;

        case PP_NUMBER_EXP:
            // An exponent must be followed by sign
            if (cp == '+' || cp == '-')
                NEXT_STATE(PP_NUMBER);
            else
                // This is not an exponent and should be parsed as a
                // pp-number followed by an identifier-nondigit
                SET_STATE(PP_NUMBER);

            break;

        case PP_NUMBER_FRAC:
            // An optional fraction must be followed by a digit
            if (IS_DIGIT(cp))
                NEXT_STATE(PP_NUMBER);
            else
                // This can't be a number fraction
                SET_STATE(PTOKEN_START);

            break;

        case CHAR_LITERAL:
            // Continues reading until a quote, backslash, or new-line
            if (cp == '\n')
                throw runtime_error("unterminated character literal");
            else if (cp == '\\')
                CALL_STATE(ESC_SEQUENCE);
            else if (cp == '\'')
                NEXT_STATE(CHAR_LITERAL_MAYBE_USER);
            else
                mForward++;

            break;

        case CHAR_LITERAL_MAYBE_USER:
            // Check if the next character can start an identifier
            if (IS_IDNONDIGIT(cp))
                NEXT_STATE(USER_CHAR_LITERAL);
            else
                EMIT_TOKEN(character_literal, mForward);

            break;

        case USER_CHAR_LITERAL:
            // Continue reading an identifier
            if (IS_DIGIT(cp) || IS_IDNONDIGIT(cp))
                mForward++;
            else
                EMIT_TOKEN(user_defined_character_literal, mForward);

            break;

        case STRING_LITERAL:
            // Continues reading until a quote, backslash, or new-line
            if (cp == '\n')
                throw runtime_error("unterminated string literal");
            else if (cp == '\\')
                CALL_STATE(ESC_SEQUENCE);
            else if (cp == '"')
                NEXT_STATE(STRING_LITERAL_MAYBE_USER);
            else
                mForward++;

            break;

        case STRING_LITERAL_MAYBE_USER:
            // Check if the next character can start an identifier
            if (IS_IDNONDIGIT(cp))
                NEXT_STATE(USER_STRING_LITERAL);
            else
                EMIT_TOKEN(string_literal, mForward);

            break;

        case USER_STRING_LITERAL:
            // Continue reading as long as the character can be in an
            // identifier
            if (IS_DIGIT(cp) || IS_IDNONDIGIT(cp))
                mForward++;
            else
                EMIT_TOKEN(user_defined_string_literal, mForward);

            break;

        case ENCODING_PREFIX:
            // Must be followed by a single quote, double quote, 8 for UTF8,
            // or an R to be a literal
            if (cp == '\'')
            {
                // Encoding prefix u8 is only valid for string literals
                if (mForward > 1 && mCpStream.substr(0, 2) == U"u8")
                    BACK_STATE(IDENTIFIER, mForward-2);
                else
                    NEXT_STATE(CHAR_LITERAL);
            }
            else if (cp == '"')
                NEXT_STATE(STRING_LITERAL);
            else if (cp == '8' && mForward > 0 && \
                    mCpStream[mForward-1] == 'u')
                mForward++;
            else if (cp == 'R')
                NEXT_STATE(RAW_STRING_LITERAL_START);
            else
                // Maybe this is an identifier
                SET_STATE(IDENTIFIER);

            break;

        case RAW_STRING_LITERAL_START:
            // This needs to be a double quote or we don't have a raw string
            if (cp == '"')
            {
                mTranslate = false;
                NEXT_STATE(RAW_STRING_LITERAL_DELIM);
            }
            else
                SET_STATE(IDENTIFIER);

            break;

        case RAW_STRING_LITERAL_DELIM:
            // Consume up to 16 characters until we find an open parenthesis
            // which signals the start of the string.  This cannot be a
            // space, left parenthesis, right parenthesis, backslash,
            // horizontal tab, vertical tab, form feed, or new-line.
            if (cp == '(')
                NEXT_STATE(RAW_STRING_LITERAL);
            else if (cp == ' ' || cp == ')' || cp == '\\' || cp == '\t' || \
                    cp == '\v' || cp == '\f' || cp == '\n')
                throw runtime_error("invalid characters in raw string delimeter");
            else
            {
                if (mRawDelim.length() >= 16)
                    throw runtime_error("raw string delimeter too long");

                mRawDelim.push_back(cp);
                mForward++;
            }

            break;

        case RAW_STRING_LITERAL:
            // We must match the entire delimeter for this to be well-formed
            if (cp == '"')
            {
                size_t index = mCpStream.rfind(')');
                if (index != string::npos &&
                    (mForward - (index+1)) <= 16 &&
                    mCpStream.substr(index+1, (mForward - (index+1))) == \
                        mRawDelim)
                {
                    mTranslate = true;
                    mRawDelim.clear();
                    EMIT_TOKEN(string_literal, mForward+1);
                }
                else
                    mForward++;
            }
            else
                mForward++;

            break;

        case PRE_OP_OR_PUNC:
            // We only get to this state when there is one character that
            // can match
            EMIT_TOKEN(preprocessing_op_or_punc, mForward);

            break;

        case PRE_OP_OR_PUNC_HASH:
            // This can only be another hash
            if (cp == '#')
                EMIT_TOKEN(preprocessing_op_or_punc, mForward+1);
            else
                EMIT_TOKEN(preprocessing_op_or_punc, mForward);

            break;

        case PRE_OP_OR_PUNC_COLON:
            // This can only be another colon or a greater-than
            if (cp == ':' || cp == '>')
                EMIT_TOKEN(preprocessing_op_or_punc, mForward+1);
            else
                EMIT_TOKEN(preprocessing_op_or_punc, mForward);

            break;

        case PRE_OP_OR_PUNC_LT:
            // This can be a colon, percent, equals, or another less-than
            if (cp == '%' || cp == '=')
                EMIT_TOKEN(preprocessing_op_or_punc, mForward+1);
            else if (cp == '<')
                NEXT_STATE(PRE_OP_OR_PUNC_LT_LT);
            else if (cp == ':')
                NEXT_STATE(PRE_OP_OR_PUNC_LT_COLON);
            else
                EMIT_TOKEN(preprocessing_op_or_punc, mForward);

            break;

        case PRE_OP_OR_PUNC_LT_LT:
            // This can be an equals
            if (cp == '=')
                EMIT_TOKEN(preprocessing_op_or_punc, mForward+1);
            else
                EMIT_TOKEN(preprocessing_op_or_punc, mForward);

            break;

        case PRE_OP_OR_PUNC_LT_COLON:
            // This can be another colon
            if (cp == ':')
                NEXT_STATE(PRE_OP_OR_PUNC_LT_COLON_COLON);
            else
                EMIT_TOKEN(preprocessing_op_or_punc, mForward);

            break;

        case PRE_OP_OR_PUNC_LT_COLON_COLON:
            // The specification says that if the next character is neither
            // a colon or a greater-than, then the less-than should be
            // treated as it's own symbol
            if (cp != ':' && cp != '>')
                EMIT_TOKEN(preprocessing_op_or_punc, mForward-2);
            else
                EMIT_TOKEN(preprocessing_op_or_punc, mForward-1);

            break;

        case PRE_OP_OR_PUNC_GT:
            // This can be an equals, or another greater-than
            if (cp == '=')
                EMIT_TOKEN(preprocessing_op_or_punc, mForward+1);
            else if (cp == '>')
                NEXT_STATE(PRE_OP_OR_PUNC_GT_GT);
            else
                EMIT_TOKEN(preprocessing_op_or_punc, mForward);

            break;

        case PRE_OP_OR_PUNC_GT_GT:
            // This can be an equals
            if (cp == '=')
                EMIT_TOKEN(preprocessing_op_or_punc, mForward+1);
            else
                EMIT_TOKEN(preprocessing_op_or_punc, mForward);

            break;

        case PRE_OP_OR_PUNC_PERCENT:
            // This can be a colon, an equals, or a greater-than
            if (cp == '=' || cp == '>')
                EMIT_TOKEN(preprocessing_op_or_punc, mForward+1);
            else if (cp == ':')
                NEXT_STATE(PRE_OP_OR_PUNC_PERCENT_COLON);
            else
                EMIT_TOKEN(preprocessing_op_or_punc, mForward);

            break;

        case PRE_OP_OR_PUNC_PERCENT_COLON:
            // Check if the next symbol is a percent
            if (cp == '%')
                NEXT_STATE(PRE_OP_OR_PUNC_PERCENT_COLON_PERCENT);
            else
                EMIT_TOKEN(preprocessing_op_or_punc, mForward);

            break;

        case PRE_OP_OR_PUNC_PERCENT_COLON_PERCENT:
            // If this isn't a colon then we have to back up the forward
            if (cp == ':')
                EMIT_TOKEN(preprocessing_op_or_punc, mForward+1);
            else
                EMIT_TOKEN(preprocessing_op_or_punc, mForward-1);

            break;

        case PRE_OP_OR_PUNC_DOT:
            // This can be another dot or a star.  If it is a digit then
            // this is a number not a preprocessing-op-or-punc token.
            if (cp == '*')
                EMIT_TOKEN(preprocessing_op_or_punc, mForward+1);
            else if (cp == '.')
                NEXT_STATE(PRE_OP_OR_PUNC_DOT_DOT);
            else if (IS_DIGIT(cp))
                SET_STATE(PP_NUMBER);
            else
                EMIT_TOKEN(preprocessing_op_or_punc, mForward);

            break;
        
        case PRE_OP_OR_PUNC_DOT_DOT:
            // If this isn't another dot we have to back up the forward
            if (cp == '.')
                EMIT_TOKEN(preprocessing_op_or_punc, mForward+1);
            else
                EMIT_TOKEN(preprocessing_op_or_punc, mForward-1);

            break;

        case PRE_OP_OR_PUNC_PLUS:
            // This can be another plus or an equals
            if (cp == '+' || cp == '=')
                EMIT_TOKEN(preprocessing_op_or_punc, mForward+1);
            else
                EMIT_TOKEN(preprocessing_op_or_punc, mForward);

            break;

        case PRE_OP_OR_PUNC_MINUS:
            // This can be another minus, an equals, or a less-than
            if (cp == '-' || cp == '=')
                EMIT_TOKEN(preprocessing_op_or_punc, mForward+1);
            else if (cp == '>')
                NEXT_STATE(PRE_OP_OR_PUNC_MINUS_LT);
            else
                EMIT_TOKEN(preprocessing_op_or_punc, mForward);

            break;

        case PRE_OP_OR_PUNC_MINUS_LT:
            // This can be a star
            if (cp == '*')
                EMIT_TOKEN(preprocessing_op_or_punc, mForward+1);
            else
                EMIT_TOKEN(preprocessing_op_or_punc, mForward);

            break;

        case PRE_OP_OR_PUNC_STAR:
            // This can be an equals
            if (cp == '=')
                EMIT_TOKEN(preprocessing_op_or_punc, mForward+1);
            else
                EMIT_TOKEN(preprocessing_op_or_punc, mForward);

            break;

        case PRE_OP_OR_PUNC_SLASH:
            // This can be an equals
            if (cp == '=')
                EMIT_TOKEN(preprocessing_op_or_punc, mForward+1);
            else
                EMIT_TOKEN(preprocessing_op_or_punc, mForward);

            break;

        case PRE_OP_OR_PUNC_PIPE:
            // This can be an euqals or another pipe
            if (cp == '=' || cp == '|')
                EMIT_TOKEN(preprocessing_op_or_punc, mForward+1);
            else
                EMIT_TOKEN(preprocessing_op_or_punc, mForward);

            break;

        case PRE_OP_OR_PUNC_CAROT:
            // This can be an equals
            if (cp == '=')
                EMIT_TOKEN(preprocessing_op_or_punc, mForward+1);
            else
                EMIT_TOKEN(preprocessing_op_or_punc, mForward);

            break;

        case PRE_OP_OR_PUNC_BANG:
            // This can be an equals
            if (cp == '=')
                EMIT_TOKEN(preprocessing_op_or_punc, mForward+1);
            else
                EMIT_TOKEN(preprocessing_op_or_punc, mForward);

            break;

        case PRE_OP_OR_PUNC_AMP:
            // This can be an equals or another ampersand
            if (cp == '=' || cp == '&')
                EMIT_TOKEN(preprocessing_op_or_punc, mForward+1);
            else
                EMIT_TOKEN(preprocessing_op_or_punc, mForward);

            break;

        case PRE_OP_OR_PUNC_EQUALS:
            // This can be another equals
            if (cp == '=')
                EMIT_TOKEN(preprocessing_op_or_punc, mForward+1);
            else
                EMIT_TOKEN(preprocessing_op_or_punc, mForward);

            break;

        case COMMENT:
            // Check if this is a single line or multi-line comment
            if (cp == '/')
                NEXT_STATE(COMMENT_ONELINE);
            else if (cp == '*')
                NEXT_STATE(COMMENT_MULTILINE);
            else
                // This isn't a comment
                SET_STATE(PRE_OP_OR_PUNC_SLASH);

            break;

        case COMMENT_ONELINE:
            // Consume all characters until a new-line
            if ((int)cp == EndOfFile)
                SET_STATE(WHITESPACE_SEQ);
            else if (cp == '\n')
                SET_STATE(WHITESPACE_SEQ);
            else
                mForward++;

            break;

        case COMMENT_MULTILINE:
            // Consume all characters until we reach an asterisk
            if (cp == '*')
                NEXT_STATE(COMMENT_MULTILINE_2);
            else if ((int)cp == EndOfFile)
                throw runtime_error("partial comment");
            else
                mForward++;

            break;

        case COMMENT_MULTILINE_2:
            // Check if this terminates the comment
            if (cp == '/')
                NEXT_STATE(WHITESPACE_SEQ);
            else if (cp != '*')
                NEXT_STATE(COMMENT_MULTILINE);
            else if ((int)cp == EndOfFile)
                throw runtime_error("partial comment");
            else
                mForward++;

            break;

        case WHITESPACE_SEQ:
            // Continue consuming until a non-whitespace character is found
            if (cp == ' ' || cp == '\t' || cp == '\v')
                mForward++;
            else
            {
                if (mLastToken != WHITESPACE_SEQ)
                    output.emit_whitespace_sequence();

                RESET_STATE(mForward);
            }

            break;

        case NEW_LINE:
            // No further states
            output.emit_new_line();
            RESET_STATE(1);

            break;

        case ESC_SEQUENCE:
            // This could be a simple escape sequence, an octal escape
            // sequence, a hexadecimal escape sequence, or a ucn
            if (SimpleEscapeSequence_CodePoints.find(cp) != \
                    SimpleEscapeSequence_CodePoints.end())
            {
                mForward++;
                RETURN_STATE();
            }
            else if (cp >= '0' && cp <= '7')
            {
                mForward++;
                RETURN_STATE();
            }
            else if (cp == 'x')
                NEXT_STATE(ESC_SEQUENCE_HEX);
            else if (cp == 'u')
                NEXT_STATE(ESC_SEQUENCE_UCN_4);
            else
                // This is an invalid escape sequence
                throw runtime_error("invalid escape sequence");

            break;

        case ESC_SEQUENCE_HEX:
            // This must be a hexadecimal character
            if (!IS_HEXDIGIT(cp))
                throw runtime_error("invalid hex escape sequence");

            // There is no maximum number of hex characters that can follow
            // so after we match one we will just return
            mForward++;
            RETURN_STATE();

            break;

        case ESC_SEQUENCE_UCN_4:
        case ESC_SEQUENCE_UCN_3:
        case ESC_SEQUENCE_UCN_2:
        case ESC_SEQUENCE_UCN_1:
            // This must be a hexadecimal character
            if (!IS_HEXDIGIT(cp))
                throw runtime_error("invalid escape sequence");

            mForward++;

            // Continue to the next state unless this is the last
            if (mState == ESC_SEQUENCE_UCN_1)
                RETURN_STATE();
            else
                mState++;

            break;

        default:
            // We should never get here!
            throw runtime_error("Bad tokenization state");
        }
    }
}
