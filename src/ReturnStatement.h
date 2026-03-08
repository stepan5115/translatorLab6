#ifndef RETURNSTATEMENT_H
#define RETURNSTATEMENT_H

#include <any>
#include <memory>
#include "Parameter.h"
#include "Expression.h"

class Context;
class Expression;

class ReturnStatement {
private:
    std::unique_ptr<Expression> returnValue;
    
public:
    ReturnStatement() : returnValue(nullptr) {}
    ReturnStatement(std::unique_ptr<Expression> expr) : returnValue(std::move(expr)) {}
    
    std::pair<Type, std::any> evaluate(Context& context);
    
    bool hasReturnValue() const {
        return returnValue != nullptr;
    }
};

#endif