#include <iostream>
#include <string>
#include <map>
#include "token.h"
#include "ast.h"
#include "parser.h"
#include "mainloop.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"

std::string IdentifierStr;
double NumVal;
int CurTok;
std::map<char, int> BinopPrecedence;

std::unique_ptr<llvm::LLVMContext> TheContext;
std::unique_ptr<llvm::IRBuilder<>> Builder;
std::unique_ptr<llvm::Module> TheModule;
std::map<std::string, llvm::Value*> NamedValues;

int main(void)
{
	BinopPrecedence['<'] = 10;
	BinopPrecedence['+'] = 20;
	BinopPrecedence['-'] = 30;
	BinopPrecedence['*'] = 40;

	fprintf(stderr, "ready> ");
	getNextToken();

	MainLoop();

	return 0;
}

