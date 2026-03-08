#include "Function.h"
#include "Context.h"
#include "Statement.h"
#include "ReturnStatement.h"
#include "Expression.h"

Function::Function(Type returnType, const std::vector<Parameter>& params)
    : returnType(returnType), parameters(params), returnStmt(nullptr) {}

Function::~Function() = default;

void Function::addStatement(std::unique_ptr<Statement> stmt) {
    body.push_back(std::move(stmt));
}

void Function::setReturnStatement(std::unique_ptr<ReturnStatement> stmt) {
    returnStmt = std::move(stmt);
}

void Function::executeBody(Context& context){
    for (auto& stmt : body) {
        stmt->execute(context);
    }
}

std::pair<Type, std::any> Function::handleReturn(Context& context) {
    if (returnStmt) {
        return returnStmt->evaluate(context);
    }
    return {VOID, {}};
}

std::pair<Type, std::any> Function::execute(Context& context, 
                                           const std::string& functionName,
                                           const std::vector<std::unique_ptr<Expression>>& args) {
    if (args.size() != parameters.size()) {
         throw std::runtime_error(std::string("Функция '") + functionName + "' ожидает " + std::to_string(parameters.size()) +
            " аргумент(ов), но получила " + std::to_string(args.size()));
    }
    
    std::vector<std::pair<Type, std::any>> argValues;
    for (size_t i = 0; i < args.size(); ++i) {
        try {
            argValues.push_back(args[i]->evaluate(context));
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Ошибка вычисления аргумента ") + std::to_string(i + 1) +
                " функции '" + functionName + "': " + e.what());
        }
    }
    
    for (size_t i = 0; i < argValues.size(); ++i) {
        if (argValues[i].first != parameters[i].getType()) {
            throw std::runtime_error(std::string("Функция '") + functionName + "': аргумент " + std::to_string(i + 1) +
                " (" + parameters[i].getName() + ") должен быть типа " + (parameters[i].getType() == INT ? "int" : "void") +
                ", но получен " + (argValues[i].first == INT ? "int" : "void"));
        }
    }
    
    context.pushScope();
    for (size_t i = 0; i < parameters.size(); ++i) {
        context.setVariable(parameters[i].getName(), argValues[i].first, argValues[i].second);
    }

    executeBody(context);
    auto result = handleReturn(context);
    bool hasReturn = returnStmt != nullptr;

    context.popScope();
    if (returnType == VOID) {
        if (hasReturn && result.first != VOID) {
            throw std::runtime_error("Функция '" + functionName + "' объявлена как void, но вернула значение");
        }
        return {VOID, {}};
    } else {
        if (!hasReturn) {
            throw std::runtime_error("Функция '" + functionName + "' должна вернуть значение, но return отсутствует");
        }
        if (result.first != INT) {
            throw std::runtime_error("Функция '" + functionName + "' должна вернуть int");
        }
        try {
            std::any_cast<int>(result.second);
        } catch (const std::bad_any_cast&) {
            throw std::runtime_error("Функция '" + functionName + "' вернула не int значение");
        }
        return result;
    }
}

void PrintFunction::executeBody(Context& context) {
    auto value = context.getVariable("value");
    if (value.first == INT) {
        std::cout << std::any_cast<int>(value.second) << std::endl;
    }
}

std::pair<Type, std::any> AbsFunction::handleReturn(Context& context) {
    int x = std::any_cast<int>(context.getVariable("x").second);
    int result = x < 0 ? -x : x;
    return {INT, result};
}

std::pair<Type, std::any> MinFunction::handleReturn(Context& context) {
    int a = std::any_cast<int>(context.getVariable("a").second);
    int b = std::any_cast<int>(context.getVariable("b").second);
    return {INT, a < b ? a : b};
}

std::pair<Type, std::any> MaxFunction::handleReturn(Context& context) {
    int a = std::any_cast<int>(context.getVariable("a").second);
    int b = std::any_cast<int>(context.getVariable("b").second);
    return {INT, a > b ? a : b};
}

std::pair<Type, std::any> PowFunction::handleReturn(Context& context) {
    int base = std::any_cast<int>(context.getVariable("base").second);
    int exp = std::any_cast<int>(context.getVariable("exp").second);
    
    int result = 1;
    for (int i = 0; i < exp; ++i) {
        result *= base;
    }
    return {INT, result};
}

std::pair<Type, std::any> SquareFunction::handleReturn(Context& context) {
    int x = std::any_cast<int>(context.getVariable("x").second);
    return {INT, x * x};
}

std::pair<Type, std::any> FactorialFunction::handleReturn(Context& context) {
    int n = std::any_cast<int>(context.getVariable("n").second);
    if (n < 0) throw std::runtime_error("Факториал от отрицательного числа");
    
    int result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return {INT, result};
}

std::pair<Type, std::any> IsEvenFunction::handleReturn(Context& context) {
    int x = std::any_cast<int>(context.getVariable("x").second);
    return {INT, x % 2 == 0 ? 1 : 0};
}