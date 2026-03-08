#include "ReturnStatement.h"
#include "Expression.h"
#include "Context.h"

std::pair<Type, std::any> ReturnStatement::evaluate(Context& context) {

    if (returnValue) {
        return returnValue->evaluate(context);
    }

    return {VOID, {}};
}