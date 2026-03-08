#include "Function.h"
#include "Statement.h"
#include <string>

Function::Function(const std::string& name, Type returnType, const std::vector<Parameter>& params)
    : name(name), returnType(returnType), parameters(params), returnStmt(nullptr) {}

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
                                           const std::vector<std::unique_ptr<Expression>>& args) {
    if (args.size() != parameters.size()) {
         throw std::runtime_error(std::string("Функция '") + name + "' ожидает " + std::to_string(parameters.size()) +
            " аргумент(ов), но получила " + std::to_string(args.size()));
    }
    
    std::vector<std::pair<Type, std::any>> argValues;
    for (size_t i = 0; i < args.size(); ++i) {
        try {
            argValues.push_back(args[i]->evaluate(context));
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Ошибка вычисления аргумента ") + std::to_string(i + 1) +
                " функции '" + name + "': " + e.what());
        }
    }
    
    for (size_t i = 0; i < argValues.size(); ++i) {
        if (argValues[i].first != parameters[i].getType()) {
            throw std::runtime_error(std::string("Функция '") + name + "': аргумент " + std::to_string(i + 1) +
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
            throw std::runtime_error("Функция '" + name + "' объявлена как void, но вернула значение");
        }
        return {VOID, {}};
    } else {
        if (!hasReturn) {
            throw std::runtime_error("Функция '" + name + "' должна вернуть значение, но return отсутствует");
        }
        if (result.first != INT) {
            throw std::runtime_error("Функция '" + name + "' должна вернуть int");
        }
        try {
            std::any_cast<int>(result.second);
        } catch (const std::bad_any_cast&) {
            throw std::runtime_error("Функция '" + name + "' вернула не int значение");
        }
        return result;
    }
}