#include <iostream>
#include <sstream>
#include <fstream>

#include "pp.h"

struct DebugPPTokenStream : IPPTokenStream
{
	void emit_whitespace_sequence()
	{
		cout << "whitespace-sequence 0 " << endl;
	}

	void emit_new_line()
	{
		cout << "new-line 0 " << endl;
	}

	void emit_header_name(const string& data)
	{
		write_token("header-name", data);
	}

	void emit_identifier(const string& data)
	{
		write_token("identifier", data);
	}

	void emit_pp_number(const string& data)
	{
		write_token("pp-number", data);
	}

	void emit_character_literal(const string& data)
	{
		write_token("character-literal", data);
	}

	void emit_user_defined_character_literal(const string& data)
	{
		write_token("user-defined-character-literal", data);
	}

	void emit_string_literal(const string& data)
	{
		write_token("string-literal", data);
	}

	void emit_user_defined_string_literal(const string& data)
	{
		write_token("user-defined-string-literal", data);
	}

	void emit_preprocessing_op_or_punc(const string& data)
	{
		write_token("preprocessing-op-or-punc", data);
	}

	void emit_non_whitespace_char(const string& data)
	{
		write_token("non-whitespace-character", data);
	}

	void emit_eof()
	{
		cout << "eof" << endl;
	}

private:

	void write_token(const string& type, const string& data)
	{
		cout << type << " " << data.size() << " ";
		cout.write(data.data(), data.size());
		cout << endl;
	}
};

int main()
{
    try
    {
        ostringstream oss;
        oss << cin.rdbuf();

        string input = oss.str();

        DebugPPTokenStream output;

        PPTokenizer tokenizer(output);

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
