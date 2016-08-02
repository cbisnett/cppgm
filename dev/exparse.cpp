// (C) 2013 CPPGM Foundation www.cppgm.org.  All rights reserved.

#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "token.h"
#include "exparse.h"

using namespace std;

// mock implementation of IsDefinedIdentifier for PA3
// return true iff first code point is odd
bool PA3Mock_IsDefinedIdentifier(const string& identifier)
{
	if (identifier.empty())
		return false;
	else
		return identifier[0] % 2;
}

class TokenElement
{
public:
	TokenElement(PPToken t, string v) : type(t), value(v) {}

	PPToken type;
	string value;
};

class CtrlExprParser
{
public:
	CtrlExprParser() {}
	typedef vector<TokenElement> TokenList;

	void addToken(PPToken tokenType, string value)
	{
		mTokens.push_back(TokenElement(tokenType, value));
	}

	bool isEmpty() { return mTokens.empty(); }

	void reset()
	{
		mTokens.clear();
	}

	bool evaluate(unsigned long long *result, bool *isSigned)
	{
		*result = 0;
		*isSigned = true;

		// Verify there are tokens
		if (mTokens.size() == 0)
			return false;

		// Append the sentinal for the end of the stream
		mTokens.push_back(TokenElement(TK_END, ""));

		// Start parsing
		mCur = 0;
		return parseControllingExpr(result, isSigned);
	}

protected:
	bool parseControllingExpr(unsigned long long *result, bool *isSigned)
	{
		if (parseLogicalOrExpr(result, isSigned))
			return true;

		return false;
	}

	bool parseLogicalOrExpr(unsigned long long *result, bool *isSigned)
	{
		if (parseLogicalAndExpr(result, isSigned))
			return true;

		return false;
	}

	bool parseLogicalAndExpr(unsigned long long *result, bool *isSigned)
	{
		if (parseInclusiveOrExpr(result, isSigned))
			return true;

		return false;
	}

	bool parseInclusiveOrExpr(unsigned long long *result, bool *isSigned)
	{
		if (parseExclusiveOrExpr(result, isSigned))
			return true;

		return false;
	}

	bool parseExclusiveOrExpr(unsigned long long *result, bool *isSigned)
	{
		if (parseAndExpr(result, isSigned))
			return true;

		return false;
	}

	bool parseAndExpr(unsigned long long *result, bool *isSigned)
	{
		if (parseEqualityExpr(result, isSigned))
			return true;

		return false;
	}

	bool parseEqualityExpr(unsigned long long *result, bool *isSigned)
	{
		if (parseRelationalExpr(result, isSigned))
			return true;

		return false;
	}

	bool parseRelationalExpr(unsigned long long *result, bool *isSigned)
	{
		if (parseShiftExpr(result, isSigned))
			return true;

		return false;
	}

	bool parseShiftExpr(unsigned long long *result, bool *isSigned)
	{
		if (parseAdditiveExpr(result, isSigned))
			return true;

		return false;
	}

	bool parseAdditiveExpr(unsigned long long *result, bool *isSigned)
	{
		if (parseMultiplicativeExpr(result, isSigned))
			return true;

		return false;
	}

	bool parseMultiplicativeExpr(unsigned long long *result, bool *isSigned)
	{
		if (parseUnaryExpr(result, isSigned))
			return true;

		return false;
	}

	bool parseUnaryExpr(unsigned long long *result, bool *isSigned)
	{
		if (parsePrimaryExpr(result, isSigned))
			return true;

		return false;
	}

	bool parsePrimaryExpr(unsigned long long *result, bool *isSigned)
	{
		char *endPtr = nullptr;

		// Check if this is a integral-literal
		if (mTokens[mCur].type == TK_PPNUMBER)
		{
			// Try and convert to an unsigned long long
			errno = 0;
			*result = strtoull(mTokens[mCur].value.c_str(), &endPtr, 0);

			// Check for failure
			if ((*result == 0 && errno != 0) ||
					(endPtr - mTokens[mCur].value.c_str()) < mTokens[mCur].value.length())
				throw runtime_error("malformed number");
		}
		return false;
	}

	TokenList mTokens;
	unsigned int mCur;
};


CtrlExpr::CtrlExpr(IPPTokenStream& output)
	: mOutput(output)
{
	mParser = new CtrlExprParser();
}

void CtrlExpr::emit_whitespace_sequence()
{
	// Ignore these
}

void CtrlExpr::emit_new_line()
{
	eval_expr();
}

void CtrlExpr::emit_header_name(const string& data)
{

}

void CtrlExpr::emit_identifier(const string& data)
{
	mParser->addToken(TK_IDENTIFIER, data);
}

void CtrlExpr::emit_pp_number(const string& data)
{
	mParser->addToken(TK_PPNUMBER, data);
}

void CtrlExpr::emit_character_literal(const string& data)
{
	mParser->addToken(TK_CHARLITERAL, data);
}

void CtrlExpr::emit_user_defined_character_literal(const string& data)
{
	mParser->addToken(TK_USER_CHARLITERAL, data);
}

void CtrlExpr::emit_string_literal(const string& data)
{
	mParser->addToken(TK_STRINGLITERAL, data);
}

void CtrlExpr::emit_user_defined_string_literal(const string& data)
{
	mParser->addToken(TK_USER_STRINGLITERAL, data);
}

void CtrlExpr::emit_preprocessing_op_or_punc(const string& data)
{
	mParser->addToken(TK_PREPROC, data);
}

void CtrlExpr::emit_non_whitespace_char(const string& data)
{

}

void CtrlExpr::emit_eof()
{
	eval_expr();
	mOutput.emit_eof();
}

void CtrlExpr::eval_expr()
{
	unsigned long long result;
	bool isSigned;

	if (mParser->isEmpty())
		return;

	try
	{
		mParser->evaluate(&result, &isSigned);

		cout << result;
		if (!isSigned)
			cout << "u";
		cout << endl;
	}
	catch (exception& e)
	{
		cerr << "ERROR: " << e.what() << endl;
		cout << "error" << endl;
	}

	mParser->reset();
}
