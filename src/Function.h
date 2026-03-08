#ifndef FUNCTION_H
#define FUNCTION_H

#include <string>
#include <vector>
#include <memory>
#include <any>
#include "Parameter.h"
#include "Context.h"
#include "Expression.h"

class Statement;
class ReturnStatement;

class Function {
private:
    const std::string name;
    const Type returnType;
    const std::vector<Parameter> parameters;
    std::unique_ptr<ReturnStatement> returnStmt;
protected:
    std::vector<std::unique_ptr<Statement>> body;
    virtual void executeBody(Context& context);
public:
    Function(const std::string& name, Type returnType, const std::vector<Parameter>& params);
    ~Function();
    
    const std::string& getName() const { return name; }
    Type getReturnType() const { return returnType; }
    const std::vector<Parameter>& getParameters() const { return parameters; }
    
    void addStatement(std::unique_ptr<Statement> stmt);
    void setReturnStatement(std::unique_ptr<ReturnStatement> stmt);
    
    std::pair<Type, std::any> execute(Context& context, 
                                      const std::vector<std::unique_ptr<Expression>>& args);
};

#endif