#include "Expression.h"
#include "Function.h"
#include <vector>
#include <memory>
#include <string>

FunctionCallExpression::FunctionCallExpression(const std::string& name, 
                                               std::vector<std::unique_ptr<Expression>> args)
    : functionName(name), arguments(std::move(args)) {}

std::pair<Type, std::any> FunctionCallExpression::evaluate(Context& context) {
    Function* func = context.getFunction(functionName);
    if (!func) {
        throw std::runtime_error("Функция '" + functionName + "' не определена");
    }
    
    return func->execute(context, arguments);
}