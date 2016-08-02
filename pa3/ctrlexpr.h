// Interface definition for CtrlExpr parser

#pragma once

#include <string>
#include <vector>

#include "token.h"
#include "IPPTokenStream.h"
#include "posttoken.h"

using namespace std;

enum PPToken {
	TK_IDENTIFIER,
	TK_PPNUMBER,
	TK_CHARLITERAL,
	TK_USER_CHARLITERAL,
	TK_STRINGLITERAL,
	TK_USER_STRINGLITERAL,
	TK_PREPROC,
	
	TK_END,
};

class CtrlExprParser;

class CtrlExpr : public IPPTokenStream
{
public:
	CtrlExpr(IPPTokenStream& output);

	void emit_whitespace_sequence();
	void emit_new_line();
	void emit_header_name(const string& data);
	void emit_identifier(const string& data);
	void emit_pp_number(const string& data);
	void emit_character_literal(const string& data);
	void emit_user_defined_character_literal(const string& data);
	void emit_string_literal(const string& data);
	void emit_user_defined_string_literal(const string& data);
	void emit_preprocessing_op_or_punc(const string& data);
	void emit_non_whitespace_char(const string& data);
	void emit_eof();

	void eval_expr();

private:
	IPPTokenStream& mOutput;
	CtrlExprParser *mParser;
};