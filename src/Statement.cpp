#include "Statement.h"
#include "Function.h"
#include "Context.h"

void CallStatement::execute(Context& context) {
    Function* func = context.getFunction(functionName);
    if (!func) {
        throw std::runtime_error("Функция не найдена: " + functionName);
    }
    
    func->execute(context, functionName, arguments);
}