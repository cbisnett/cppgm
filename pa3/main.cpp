// Main implementation for PA3

#include <cstdio>
#include <iostream>
#include <sstream>

#include "pptoken.h"
#include "posttoken.h"
#include "ctrlexpr.h"
#include "IPPTokenStream.h"

class DebugOutput : public IPPTokenStream
{
public:
	DebugOutput() {}
	virtual ~DebugOutput() {}

	virtual void emit_whitespace_sequence()
	{

	}

	virtual void emit_new_line()
	{

	}

	virtual void emit_header_name(const string& data)
	{

	}

	virtual void emit_identifier(const string& data)
	{

	}

	virtual void emit_pp_number(const string& data)
	{

	}

	virtual void emit_character_literal(const string& data)
	{

	}

	virtual void emit_user_defined_character_literal(const string& data)
	{

	}

	virtual void emit_string_literal(const string& data)
	{

	}

	virtual void emit_user_defined_string_literal(const string& data)
	{

	}

	virtual void emit_preprocessing_op_or_punc(const string& data)
	{

	}

	virtual void emit_non_whitespace_char(const string& data)
	{

	}

	virtual void emit_eof()
	{
		cout << "eof" << endl;
	}
};

int main()
{
	DebugOutput output;
	CtrlExpr ctrlexpr(output);
	PPTokenizer tokenizer(ctrlexpr);

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