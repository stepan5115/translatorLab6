#ifndef FUNCTION_H
#define FUNCTION_H

#include <string>
#include <vector>
#include <memory>
#include <any>
#include "Parameter.h"
#include "Context.h"
#include "Expression.h"

class Statement;
class ReturnStatement;

class Function {
private:
    const std::string name;
    const Type returnType;
    const std::vector<Parameter> parameters;
protected:
    std::vector<std::unique_ptr<Statement>> body;
    std::unique_ptr<ReturnStatement> returnStmt;
    virtual void executeBody(Context& context);
    virtual std::pair<Type, std::any> handleReturn(Context& context);
public:
    Function(const std::string& name, Type returnType, const std::vector<Parameter>& params);
    ~Function();
    
    const std::string& getName() const { return name; }
    Type getReturnType() const { return returnType; }
    const std::vector<Parameter>& getParameters() const { return parameters; }
    
    void addStatement(std::unique_ptr<Statement> stmt);
    void setReturnStatement(std::unique_ptr<ReturnStatement> stmt);
    
    std::pair<Type, std::any> execute(Context& context, 
                                      const std::vector<std::unique_ptr<Expression>>& args);
};

class PrintFunction : public Function {
public:
    PrintFunction() : Function("print", VOID, {Parameter(INT, "value")}) {} 
protected:
    void executeBody(Context& context) override {
        auto value = context.getVariable("value");
        if (value.first == INT) {
            std::cout << std::any_cast<int>(value.second) << std::endl;
        }
    }
};

class AbsFunction : public Function {
public:
    AbsFunction() : Function("abs", INT, {Parameter(INT, "x")}) {}
protected:
    std::pair<Type, std::any> handleReturn(Context& context) override {
        int x = std::any_cast<int>(context.getVariable("x").second);
        int result = x < 0 ? -x : x;
        return {INT, result};
    }
};

class MinFunction : public Function {
public:
    MinFunction() : Function("min", INT, {Parameter(INT, "a"), Parameter(INT, "b")}) {}
protected:
    std::pair<Type, std::any> handleReturn(Context& context) override {
        int a = std::any_cast<int>(context.getVariable("a").second);
        int b = std::any_cast<int>(context.getVariable("b").second);
        return {INT, a < b ? a : b};
    }
};

class MaxFunction : public Function {
public:
    MaxFunction() : Function("max", INT, {Parameter(INT, "a"), Parameter(INT, "b")}) {}
protected:
    std::pair<Type, std::any> handleReturn(Context& context) override {
        int a = std::any_cast<int>(context.getVariable("a").second);
        int b = std::any_cast<int>(context.getVariable("b").second);
        return {INT, a > b ? a : b};
    }
};

class PowFunction : public Function {
public:
    PowFunction() : Function("pow", INT, {Parameter(INT, "base"), Parameter(INT, "exp")}) {}
protected:
    std::pair<Type, std::any> handleReturn(Context& context) override {
        int base = std::any_cast<int>(context.getVariable("base").second);
        int exp = std::any_cast<int>(context.getVariable("exp").second);
        
        int result = 1;
        for (int i = 0; i < exp; ++i) {
            result *= base;
        }
        return {INT, result};
    }
};

class SquareFunction : public Function {
public:
    SquareFunction() : Function("sqr", INT, {Parameter(INT, "x")}) {}
protected:
    std::pair<Type, std::any> handleReturn(Context& context) override {
        int x = std::any_cast<int>(context.getVariable("x").second);
        return {INT, x * x};
    }
};

class FactorialFunction : public Function {
public:
    FactorialFunction() : Function("fact", INT, {Parameter(INT, "n")}) {}
protected:
    std::pair<Type, std::any> handleReturn(Context& context) override {
        int n = std::any_cast<int>(context.getVariable("n").second);
        if (n < 0) throw std::runtime_error("Факториал от отрицательного числа");
        
        int result = 1;
        for (int i = 2; i <= n; ++i) {
            result *= i;
        }
        return {INT, result};
    }
};

class IsEvenFunction : public Function {
public:
    IsEvenFunction() : Function("isEven", INT, {Parameter(INT, "x")}) {}
protected:
    std::pair<Type, std::any> handleReturn(Context& context) override {
        int x = std::any_cast<int>(context.getVariable("x").second);
        return {INT, x % 2 == 0 ? 1 : 0};  // возвращаем 1 или 0 как bool
    }
};

#endif