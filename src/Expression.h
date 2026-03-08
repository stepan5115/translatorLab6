#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <any>
#include <memory>
#include <vector>
#include "Parameter.h"

class Context;

class Expression {
public:
    virtual ~Expression() = default;
    virtual std::pair<Type, std::any> evaluate(Context& context) = 0;
};

class IntExpression : public Expression {
    int value;
public:
    IntExpression(int v) : value(v) {}
    std::pair<Type, std::any> evaluate(Context&) override {
        return {INT, value};
    }
};

class VariableExpression : public Expression {
    std::string name;
public:
    VariableExpression(const std::string& n) : name(n) {}
    std::pair<Type, std::any> evaluate(Context& context) override;
};

class FunctionCallExpression : public Expression {
private:
    std::string functionName;
    std::vector<std::unique_ptr<Expression>> arguments;
public:
    FunctionCallExpression(const std::string& name, std::vector<std::unique_ptr<Expression>> args) : functionName(name), arguments(std::move(args)) {}
    std::pair<Type, std::any> evaluate(Context& context) override;
    const std::string& getFunctionName() const { return functionName; }
    size_t getArgumentCount() const { return arguments.size(); }
    std::vector<std::unique_ptr<Expression>> releaseArguments() {
        return std::move(arguments);
    }
};

class BinaryExpression : public Expression {
    std::unique_ptr<Expression> left, right;
    char op;
public:
    BinaryExpression(std::unique_ptr<Expression> l, std::unique_ptr<Expression> r, char o)
        : left(std::move(l)), right(std::move(r)), op(o) {}
    
    std::pair<Type, std::any> evaluate(Context& context) override;
};

#endif