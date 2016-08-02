#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <stdexcept>

#include "token.h"
#include "pp.h"
#include "post.h"
#include "exparse.h"

class DebugCtrlExprOutputStream : public IPPTokenStream
{
public:
    void emit_whitespace_sequence()
    {
    }

    void emit_new_line()
    {
    }

    void emit_header_name(const string& data)
    {
    }

    void emit_identifier(const string& data)
    {
    }

    void emit_pp_number(const string& data)
    {
    }

    void emit_character_literal(const string& data)
    {
    }

    void emit_user_defined_character_literal(const string& data)
    {
    }

    void emit_string_literal(const string& data)
    {
    }

    void emit_user_defined_string_literal(const string& data)
    {
    }

    void emit_preprocessing_op_or_punc(const string& data)
    {
    }

    void emit_non_whitespace_char(const string& data)
    {
    }

    void emit_eof()
    {
        cout << "eof" << endl;
    }
};

int main()
{
    DebugCtrlExprOutputStream output;
    CtrlExpr exparser(output);
    PPTokenizer tokenizer(exparser);

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
}
