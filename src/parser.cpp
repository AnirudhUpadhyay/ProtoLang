#include "parser.h"
#include "token.h"
#include <string>
#include <map>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"

//Global state.
extern std::string IdentifierStr;
extern int CurTok;
extern double NumVal;
extern std::map<char, int> BinopPrecedence;

//Global state.
extern std::unique_ptr<llvm::LLVMContext> TheContext;
extern std::unique_ptr<llvm::IRBuilder<>> Builder;
extern std::unique_ptr<llvm::Module> TheModule;

//Error handling functions.
std::unique_ptr<ExprAST> LogError(const char* Str)
{
	fprintf(stderr, "Error: %s\n", Str);
	return nullptr;
}

std::unique_ptr<PrototypeAST> LogErrorP(const char* Str)
{
	LogError(Str);
	return nullptr;
}

//Parsing functions.
std::unique_ptr<ExprAST> ParseNumberExpr()
{
	auto result = std::make_unique<NumberExprAST> (NumVal);
	getNextToken(); //consume the number
	return std::move(result);
}

std::unique_ptr<ExprAST> ParseParenExpr()
{
	//eat '('.
	getNextToken();
	
	auto v = ParseExpression();
	if(!v)
	{
		return nullptr;
	}

	if(CurTok != ')')
		return LogError("expected ')'");
	getNextToken(); //eat ')'.
	return v;
}

std::unique_ptr<ExprAST> ParseIdentifierExpr()
{
	std::string IdName = IdentifierStr;

	getNextToken(); //eat identifier.

	if (CurTok != '(')
	{
		return std::make_unique<VariableExprAST>(IdName);
	}

	getNextToken(); //eat '('.

	std::vector<std::unique_ptr<ExprAST>> Args;

	if (CurTok != ')')
	{
		while(true)
		{
			if (auto Arg = ParseExpression()) Args.push_back(std::move(Arg));
			else return nullptr;

			if (CurTok == ')') break;

			if (CurTok != ',') return LogError("Expected ')' or ',' in argument list");

			getNextToken();
		}
	}

	getNextToken(); //eat ')'.

	return std::make_unique<CallExprAST>(IdName, std::move(Args));
}

std::unique_ptr<ExprAST> ParsePrimary()
{
	switch (CurTok)
	{
		default:
			return LogError("Unkown token when expecting an expression");
		case tok_identifier:
			return ParseIdentifierExpr();
		case tok_number:
			return ParseNumberExpr();
		case '(':
			return ParseParenExpr();
	}
}

std::unique_ptr<ExprAST> ParseBinOpRHS(int ExprPrec, std::unique_ptr<ExprAST> LHS)
{
	while (true)
	{
		int TokPrec = GetTokPrecedence();

		if (TokPrec < ExprPrec) return LHS;

		int BinOp = CurTok;
		getNextToken(); //eat binop.

		auto RHS = ParsePrimary();
		if (!RHS) return nullptr;

		int NextPrec = GetTokPrecedence();
		if (TokPrec < NextPrec)
		{
			RHS = ParseBinOpRHS(TokPrec + 1, std::move(RHS));
			if (!RHS) return nullptr;
		}

		LHS = std::make_unique<BinaryExprAST>(BinOp, std::move(LHS), std::move(RHS));
	}
}

std::unique_ptr<ExprAST> ParseExpression()
{
	auto LHS = ParsePrimary();
	
	if (!LHS) return nullptr;

	return ParseBinOpRHS(0, std::move(LHS));
}

std::unique_ptr<PrototypeAST> ParsePrototype() {
  if (CurTok != tok_identifier)
    return LogErrorP("Expected function name in prototype");

  std::string FnName = IdentifierStr;
  getNextToken();

  if (CurTok != '(')
    return LogErrorP("Expected '(' in prototype");

  // Read the list of argument names.
  std::vector<std::string> ArgNames;
  while (getNextToken() == tok_identifier)
    ArgNames.push_back(IdentifierStr);
  if (CurTok != ')')
    return LogErrorP("Expected ')' in prototype");

  // success.
  getNextToken();  // eat ')'.

  return std::make_unique<PrototypeAST>(FnName, std::move(ArgNames));
}

std::unique_ptr<FunctionAST> ParseDefinition() {
  getNextToken();  // eat def.
  auto Proto = ParsePrototype();
  if (!Proto) return nullptr;

  if (auto E = ParseExpression())
    return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));
  return nullptr;
}

std::unique_ptr<PrototypeAST> ParseExtern() {
  getNextToken();  // eat extern.
  return ParsePrototype();
}

std::unique_ptr<FunctionAST> ParseTopLevelExpr() {
  if (auto E = ParseExpression()) {
    // Make an anonymous proto.
    auto Proto = std::make_unique<PrototypeAST>("", std::vector<std::string>());
    return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));
  }
  return nullptr;
}

//Top-level parsing functions.
void InitializeModule()
{
	TheContext = std::make_unique<llvm::LLVMContext>();
	TheModule = std::make_unique<llvm::Module>("mu cool jit", *TheContext);
	Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);
}

void HandleDefinition()
{
	if (auto FnAST = ParseDefinition())
	{
		if (auto* FnIR = FnAST->codegen())
		{
			fprintf(stderr, "Read function definiton: ");
			FnIR->print(llvm::errs());
			fprintf(stderr, "\n");
		}
	}
	else
	{
		getNextToken();
	}
}

void HandleExtern()
{
	if (auto ProtoAST = ParseExtern()) 
	{
		if (auto* FnIR = ProtoAST->codegen())
		{
			fprintf(stderr, "Read extern: ");
			FnIR->print(llvm::errs());
			fprintf(stderr, "\n");
		}
	}
	else
	{
		// Skip token for error recovery.
		getNextToken();
	}
}

void HandleTopLevelExpression() {
	// Evaluate a top-level expression into an anonymous function.
	if (auto FnAST = ParseTopLevelExpr())
	{
		if (auto* FnIR = FnAST->codegen())
		{
			fprintf(stderr, "Read top-level expression: ");
			FnIR->print(llvm::errs());
			fprintf(stderr, "\n");
			FnIR->eraseFromParent();
		}
	}
	else
	{
		getNextToken();
	}
}

int GetTokPrecedence()
{
	if (!isascii(CurTok)) return -1;

	int TokPrec = BinopPrecedence[CurTok];
	if (TokPrec <= 0) return -1;
	return TokPrec;
}
