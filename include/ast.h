#ifndef __AST_H__
#define __AST_H__

#include <string>
#include <vector>
#include <memory>
#include "llvm/IR/Value.h"
#include "llvm/IR/Function.h"

llvm::Value* LogErrorV(const char *Str);

class ExprAST
{
	public:
		virtual ~ExprAST() = default;
		virtual llvm::Value *codegen() = 0;
};

class NumberExprAST : public ExprAST
{
	private:
		double Val;

	public:
		NumberExprAST(double a_Val);
		llvm::Value *codegen() override;
};

class VariableExprAST : public ExprAST
{
	private:
		std::string Name;

	public:
		VariableExprAST(const std::string a_Name);
		llvm::Value *codegen() override;
};

class BinaryExprAST : public ExprAST
{
	private:
		char Op;
		std::unique_ptr<ExprAST> LHS, RHS;

	public:
		BinaryExprAST(char a_Op, std::unique_ptr<ExprAST> a_LHS, std::unique_ptr<ExprAST> a_RHS);
		llvm::Value *codegen() override;
};

class CallExprAST : public ExprAST
{
	private:
		std::string Callee;
		std::vector<std::unique_ptr<ExprAST>> Args;

	public:
		CallExprAST(const std::string &a_Callee, std::vector<std::unique_ptr<ExprAST>> a_Args);
		llvm::Value *codegen() override;
};

class PrototypeAST
{
	private:
		std::string Name;
		std::vector<std::string> Args;

	public:
		PrototypeAST(const std::string &a_Name, std::vector<std::string> a_Args);
		llvm::Function *codegen();
		const std::string& getName() const;
};

class FunctionAST
{
	private:
		std::unique_ptr<PrototypeAST> Proto;
		std::unique_ptr<ExprAST> Body;
		
	public:
		FunctionAST(std::unique_ptr<PrototypeAST> a_Proto, std::unique_ptr<ExprAST> Body);
		llvm::Function *codegen();
};

#endif
