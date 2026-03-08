#ifndef STATEMENT_H
#define STATEMENT_H

#include <memory>
#include <vector>
#include <string>
#include <map>
#include <any>
#include <stdexcept>
#include "Expression.h"

class Context;
class Expression;
class Function;

class Function;

class Statement {
public:
    virtual ~Statement() = default;
    virtual void execute(Context& context) = 0;
};

class CallStatement : public Statement {
private:
    std::string functionName;
    std::vector<std::unique_ptr<Expression>> arguments;
    
public:
    CallStatement(const std::string& name, std::vector<std::unique_ptr<Expression>> args)
        : functionName(name), arguments(std::move(args)) {}
    
    void execute(Context& context) override;
};

#endif