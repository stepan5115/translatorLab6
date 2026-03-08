#ifndef FUNCTION_H
#define FUNCTION_H

#include <string>
#include <vector>
#include <memory>
#include <any>
#include "Parameter.h"
#include "Statement.h"
#include "ReturnStatement.h"
#include "Expression.h"

class Context;
class Expression;
class Statement;
class ReturnStatement;

class Function {
private:
    const Type returnType;
    const std::vector<Parameter> parameters;
protected:
    std::vector<std::unique_ptr<Statement>> body;
    std::unique_ptr<ReturnStatement> returnStmt;
    virtual void executeBody(Context& context);
    virtual std::pair<Type, std::any> handleReturn(Context& context);
public:
    Function(Type returnType, const std::vector<Parameter>& params);
    ~Function();
    
    Type getReturnType() const { return returnType; }
    const std::vector<Parameter>& getParameters() const { return parameters; }
    
    void addStatement(std::unique_ptr<Statement> stmt);
    void setReturnStatement(std::unique_ptr<ReturnStatement> stmt);
    
    std::pair<Type, std::any> execute(Context& context, 
                                      const std::string& functionName,
                                      const std::vector<std::unique_ptr<Expression>>& args);
};

class PrintFunction : public Function {
public:
    PrintFunction() : Function(VOID, {Parameter(INT, "value")}) {} 
protected:
    void executeBody(Context& context) override;
};

class AbsFunction : public Function {
public:
    AbsFunction() : Function(INT, {Parameter(INT, "x")}) {}
protected:
    std::pair<Type, std::any> handleReturn(Context& context) override;
};

class MinFunction : public Function {
public:
    MinFunction() : Function(INT, {Parameter(INT, "a"), Parameter(INT, "b")}) {}
protected:
    std::pair<Type, std::any> handleReturn(Context& context) override;
};

class MaxFunction : public Function {
public:
    MaxFunction() : Function(INT, {Parameter(INT, "a"), Parameter(INT, "b")}) {}
protected:
    std::pair<Type, std::any> handleReturn(Context& context) override;
};

class PowFunction : public Function {
public:
    PowFunction() : Function(INT, {Parameter(INT, "base"), Parameter(INT, "exp")}) {}
protected:
    std::pair<Type, std::any> handleReturn(Context& context) override;
};

class SquareFunction : public Function {
public:
    SquareFunction() : Function(INT, {Parameter(INT, "x")}) {}
protected:
    std::pair<Type, std::any> handleReturn(Context& context) override;
};

class FactorialFunction : public Function {
public:
    FactorialFunction() : Function(INT, {Parameter(INT, "n")}) {}
protected:
    std::pair<Type, std::any> handleReturn(Context& context) override;
};

class IsEvenFunction : public Function {
public:
    IsEvenFunction() : Function(INT, {Parameter(INT, "x")}) {}
protected:
    std::pair<Type, std::any> handleReturn(Context& context) override;
};

#endif