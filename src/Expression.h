#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <any>
#include <memory>
#include "Context.h"

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
    std::pair<Type, std::any> evaluate(Context& context) override {
        return context.getVariable(name);
    }
};

class FunctionCallExpression : public Expression {
private:
    std::string functionName;
    std::vector<std::unique_ptr<Expression>> arguments;
public:
    FunctionCallExpression(const std::string& name, std::vector<std::unique_ptr<Expression>> args);
    std::pair<Type, std::any> evaluate(Context& context) override;
    const std::string& getFunctionName() const { return functionName; }
    size_t getArgumentCount() const { return arguments.size(); }
};

class BinaryExpression : public Expression {
    std::unique_ptr<Expression> left, right;
    char op;
public:
    BinaryExpression(std::unique_ptr<Expression> l, std::unique_ptr<Expression> r, char o)
        : left(std::move(l)), right(std::move(r)), op(o) {}
    
    std::pair<Type, std::any> evaluate(Context& context) override {
        auto lRes = left->evaluate(context);
        auto rRes = right->evaluate(context);
        
        if (lRes.first != INT || rRes.first != INT) {
            throw std::runtime_error(std::string("Бинарный оператор ") + op + " применим только к переменным типа INT");
        }
        
        int l = std::any_cast<int>(lRes.second);
        int r = std::any_cast<int>(rRes.second);
        
        switch(op) {
            case '+': return {INT, l + r};
            case '-': return {INT, l - r};
            case '*': return {INT, l * r};
            case '/': 
                if (r == 0) throw std::runtime_error("Деление на 0");
                return {INT, l / r};
            case '%':
                if (r == 0) throw std::runtime_error("Взятие остатка от деления на 0");
                return {INT, l % r};
            default: throw std::runtime_error("Неизвестный оператор");
        }
    }
};

#endif