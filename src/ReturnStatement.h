#ifndef RETURNSTATEMENT_H
#define RETURNSTATEMENT_H

#include <any>
#include <memory>
#include "Expression.h"
#include "Context.h"

class ReturnStatement {
private:
    std::unique_ptr<Expression> returnValue;
    
public:
    ReturnStatement() : returnValue(nullptr) {}
    ReturnStatement(std::unique_ptr<Expression> expr) : returnValue(std::move(expr)) {}
    
    std::pair<Type, std::any> evaluate(Context& context) {
        if (returnValue) {
            return returnValue->evaluate(context);
        } else {
            return {VOID, {}};
        }
    }
    
    bool hasReturnValue() const {
        return returnValue != nullptr;
    }
};

#endif