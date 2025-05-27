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

//Global variables for Lexer.
std::string IdentifierStr;
double NumVal;
int CurTok;
std::map<char, int> BinopPrecedence;

//Global Variables for Codegen.
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

	InitializeModule();
	MainLoop();
	TheModule->print(llvm::errs(), nullptr);

	return 0;
}
