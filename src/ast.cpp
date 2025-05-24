#include "ast.h"
#include <utility>

NumberExprAST::NumberExprAST(double a_Val): Val(a_Val) {}

VariableExprAST::VariableExprAST(const std::string a_Name): Name(a_Name) {}

BinaryExprAST::BinaryExprAST(char a_Op, std::unique_ptr<ExprAST> a_LHS, std::unique_ptr<ExprAST> a_RHS): Op(a_Op), LHS(std::move(a_LHS)), RHS(std::move(a_RHS)) {}

CallExprAST::CallExprAST(const std::string &a_Callee, std::vector<std::unique_ptr<ExprAST>> a_Args): Callee(a_Callee), Args(std::move(a_Args)) {}

PrototypeAST::PrototypeAST(const std::string &a_Name, std::vector<std::string> a_Args): Name(a_Name), Args(std::move(a_Args)) {}

FunctionAST::FunctionAST(std::unique_ptr<PrototypeAST> a_Proto, std::unique_ptr<ExprAST> a_Body): Proto(std::move(a_Proto)), Body(std::move(a_Body)) {}
