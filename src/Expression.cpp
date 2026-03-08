#include "Expression.h"
#include "Context.h"
#include "Function.h"

std::pair<Type, std::any> VariableExpression::evaluate(Context& context) {
    return context.getVariable(name);
}

std::pair<Type, std::any> FunctionCallExpression::evaluate(Context& context) {
    Function* func = context.getFunction(functionName);
    if (!func) {
        throw std::runtime_error("Функция '" + functionName + "' не определена");
    }
    
    return func->execute(context, functionName, arguments);
}

std::pair<Type, std::any> BinaryExpression::evaluate(Context& context) {

    auto lRes = left->evaluate(context);
    auto rRes = right->evaluate(context);

    if (lRes.first != INT || rRes.first != INT) {
        throw std::runtime_error(
            std::string("Бинарный оператор ") + op +
            " применим только к переменным типа INT");
    }

    int l = std::any_cast<int>(lRes.second);
    int r = std::any_cast<int>(rRes.second);

    switch (op) {
        case '+': return {INT, l + r};
        case '-': return {INT, l - r};
        case '*': return {INT, l * r};

        case '/':
            if (r == 0) throw std::runtime_error("Деление на 0");
            return {INT, l / r};

        case '%':
            if (r == 0) throw std::runtime_error("Взятие остатка от деления на 0");
            return {INT, l % r};

        default:
            throw std::runtime_error(std::string("Неизвестный оператор ") + op);
    }
}

std::pair<Type, std::any> UnaryExpr::evaluate(Context& context) {
    auto expValue = exp->evaluate(context);
    if (expValue.first != INT) {
        throw std::runtime_error(
            std::string("Унарный оператор ") + op +
            " применим только к переменным типа INT");
    }
    int value = std::any_cast<int>(expValue.second);
    switch (op) {
        case '+':
            return {INT, value};
        case '-':
            return {INT, -value};
        default:
            throw std::runtime_error(std::string("Неизвестный оператор ") + op);
    }
}