#include <stdio.h>
#include <string>
#include <cctype>
#include "token.h"


extern int CurTok;
extern std::string IdentifierStr;
extern double NumVal;

int gettok()
{
	static int LastChar = ' ';

	while(isspace(LastChar))
	{
		LastChar = getchar();
	}

	//identifier: [a-zA-Z][a-zA-Z0-9]*
	if(isalpha(LastChar))
	{
		IdentifierStr = LastChar;

		while(isalnum(LastChar = getchar()))
		{
			IdentifierStr += LastChar;
		}

		if(IdentifierStr == "def")
		{
			return tok_def;
		}
		if(IdentifierStr == "extern")
		{
			return tok_extern;
		}
		return tok_identifier;

	}

	//Number: [0-9.]+
	if(isdigit(LastChar) || LastChar == '.')
	{
		std::string NumStr;
		do
		{
			NumStr += LastChar;
			LastChar = getchar();
		}
		while(isdigit(LastChar) || LastChar == '.');

		NumVal = strtod(NumStr.c_str(), nullptr);
		return tok_number;
	}

	if(LastChar ==  '#')
	{
		do
		{
			LastChar = getchar();
		}
		while(LastChar != EOF && LastChar != '\n' && LastChar != '\r');

		if(LastChar != EOF)
		{
			return gettok();
		}

	}

	if(LastChar == EOF)
	{
		return tok_eof;
	}

	int ThisChar = LastChar;
	LastChar = getchar();
	return ThisChar;
}

int getNextToken(void)
{
	return CurTok = gettok();
}
