#include "ast.h"
#include "parser.h"
#include <utility>
#include <map>

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

//Global variables from main.cpp
extern std::unique_ptr<llvm::LLVMContext> TheContext;
extern std::unique_ptr<llvm::IRBuilder<>> Builder;
extern std::unique_ptr<llvm::Module> TheModule;
extern std::map<std::string, llvm::Value*> NamedValues;

llvm::Value* LogErrorV(const char *Str)
{
	//From parser.h
	LogError(Str);
	return nullptr;
}

NumberExprAST::NumberExprAST(double a_Val): Val(a_Val) {}

llvm::Value* NumberExprAST::codegen()
{
	return llvm::ConstantFP::get(*TheContext, llvm::APFloat(Val));
}

VariableExprAST::VariableExprAST(const std::string a_Name): Name(a_Name) {}

llvm::Value* VariableExprAST::codegen()
{
	llvm::Value* V = NamedValues[Name];
	if (!V)
	{
		LogErrorV("Unknown Variable name");
	}
	return V;
}

BinaryExprAST::BinaryExprAST(char a_Op, std::unique_ptr<ExprAST> a_LHS, std::unique_ptr<ExprAST> a_RHS): Op(a_Op), LHS(std::move(a_LHS)), RHS(std::move(a_RHS)) {}

llvm::Value* BinaryExprAST::codegen()
{
	llvm::Value* L = LHS -> codegen();
	llvm::Value* R = RHS -> codegen();

	if (!L || !R)
	{
		return nullptr;
	}

	switch (Op)
	{
	case '+':
		return Builder->CreateFAdd(L, R, "addtmp");
	case '-':
		return Builder->CreateFSub(L, R, "subtmp");
	case '*':
		return Builder->CreateFMul(L, R, "multmp");
	case '<':
		L = Builder->CreateFCmpULT(L, R, "cmptmp");
		return Builder->CreateUIToFP(L, llvm::Type::getDoubleTy(*TheContext), "booltmp");
	default:
		return LogErrorV("invalid binary operator");
	}
}

CallExprAST::CallExprAST(const std::string &a_Callee, std::vector<std::unique_ptr<ExprAST>> a_Args): Callee(a_Callee), Args(std::move(a_Args)) {}

llvm::Value* CallExprAST::codegen()
{
	llvm::Function* CalleeF = TheModule->getFunction(Callee);
	if(!CalleeF)
	{
		return LogErrorV("Unknown function referenced");
	}

	if(CalleeF->arg_size() != Args.size())
	{
		return LogErrorV("Incorrect #arguments passed");
	}

	std::vector<llvm::Value*> ArgsV;
	for (unsigned i = 0, e = Args.size(); i != e; ++i)
	{
		ArgsV.push_back(Args[i]->codegen());
		if (!ArgsV.back())
		{
			return nullptr;
		}
	}

	return Builder->CreateCall(CalleeF, ArgsV, "calltmp");
}

PrototypeAST::PrototypeAST(const std::string &a_Name, std::vector<std::string> a_Args): Name(a_Name), Args(std::move(a_Args)) {}

llvm::Function* PrototypeAST::codegen()
{
	std::vector<llvm::Type*> Doubles(Args.size(), llvm::Type::getDoubleTy(*TheContext));

	//Create function that takes Args.size() doubles and returns a double.
	//Does not have variadic Arguments (false).
	//Goes into symbol table of "TheModule"
	llvm::FunctionType* FT = llvm::FunctionType::get(llvm::Type::getDoubleTy(*TheContext), Doubles, false);
	llvm::Function* F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, Name, TheModule.get());

	unsigned Idx = 0;
	for (auto &Arg : F->args())
	{
		Arg.setName(Args[Idx++]);
	}

	return F;
}

const std::string& PrototypeAST::getName() const
{
	return Name;
}

FunctionAST::FunctionAST(std::unique_ptr<PrototypeAST> a_Proto, std::unique_ptr<ExprAST> a_Body): Proto(std::move(a_Proto)), Body(std::move(a_Body)) {}

llvm::Function* FunctionAST::codegen()
{
	llvm::Function* TheFunction = TheModule->getFunction(Proto->getName());

	if(!TheFunction)
	{
		TheFunction = Proto->codegen();
	}

	if(!TheFunction)
	{
		return nullptr;
	}

	if(!TheFunction->empty())
	{
		return (llvm::Function*) LogErrorV("Function cannot be redefined.");
	}

	llvm::BasicBlock* BB = llvm::BasicBlock::Create(*TheContext, "entry", TheFunction);
	Builder->SetInsertPoint(BB);

	NamedValues.clear();
	for (auto &Arg : TheFunction->args())
	{
		NamedValues[std::string(Arg.getName())] = &Arg;
	}

	if (llvm::Value* RetVal = Body->codegen())
	{
		//Finish the function.
		Builder->CreateRet(RetVal);
		//Validate the function.
		llvm::verifyFunction(*TheFunction);
		return TheFunction;
	}

	TheFunction->eraseFromParent();
	return nullptr;
}
