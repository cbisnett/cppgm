// Interface for PostTokenStream objects

#pragma once

#include <string>

#include "token.h"

using namespace std;

class IPostTokenStream
{
public:
	virtual void emit_invalid(const string& source) = 0;

	virtual void emit_simple(const string& source, ETokenType token_type) = 0;

	virtual void emit_identifier(const string& source) = 0;

	virtual void emit_literal(const string& source, EFundamentalType type,
		const void* data, size_t nbytes) = 0;

	virtual void emit_literal_array(const string& source, size_t num_elements,
		EFundamentalType type, const void* data, size_t nbytes) = 0;

	virtual void emit_user_defined_literal_character(const string& source,
		const string& ud_suffix, EFundamentalType type, const void* data,
		size_t nbytes) = 0;

	virtual void emit_user_defined_literal_string_array(const string& source,
		const string& ud_suffix, size_t num_elements, EFundamentalType type,
		const void* data, size_t nbytes) = 0;

	virtual void emit_user_defined_literal_integer(const string& source,
		const string& ud_suffix, const string& prefix) = 0;

 	virtual void emit_user_defined_literal_floating(const string& source,
		const string& ud_suffix, const string& prefix) = 0;
 
	virtual void emit_eof() = 0;
};
