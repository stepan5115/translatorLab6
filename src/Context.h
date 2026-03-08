#ifndef CONTEXT_H
#define CONTEXT_H

#include <iostream>
#include <any>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <stack>
#include <stdexcept>
#include "Parameter.h"

class Function;

class Scope {
private:
    std::map<std::string, std::pair<Type, std::any>> variables;
public:
    void setVariable(const std::string& name, Type type, const std::any& value) {
        variables[name] = {type, value};
    }
    bool hasVariable(const std::string& name) const {
        return variables.find(name) != variables.end();
    }
    std::pair<Type, std::any> getVariable(const std::string& name) const {
        auto it = variables.find(name);
        if (it == variables.end()) {
            throw std::runtime_error("Переменная не найдена в области видимости: " + name);
        }
        return it->second;
    }
};

class Context {
private:
    std::vector<Scope> variableScopes;
    std::map<std::string, std::unique_ptr<Function>> functions;
public:
    Context() {
        variableScopes.emplace_back();
    }
    void pushScope() {
        variableScopes.emplace_back();
    }
    void popScope() {
        if (variableScopes.size() <= 1) {
            throw std::runtime_error("Невозможно убрать локальный скоуп");
        }
        variableScopes.pop_back();
    }
    
    void setVariable(const std::string& name, Type type, const std::any& value) {
        variableScopes.back().setVariable(name, type, value);
    }
    std::pair<Type, std::any> getVariable(const std::string& name) const {
        for (auto it = variableScopes.rbegin(); it != variableScopes.rend(); ++it) {
            if (it->hasVariable(name)) {
                return it->getVariable(name);
            }
        }
        throw std::runtime_error("Переменная не найдена: " + name);
    }
    bool hasVariable(const std::string& name) const {
        for (const auto& scope : variableScopes) {
            if (scope.hasVariable(name)) {
                return true;
            }
        }
        return false;
    }
    
    void addFunction(const std::string& name, std::unique_ptr<Function> func) {
        if (functions.find(name) != functions.end()) {
            throw std::runtime_error("Функция уже определена: " + name);
        }
        functions[name] = std::move(func);
    }
    Function* getFunction(const std::string& name) const {
        auto it = functions.find(name);
        if (it == functions.end()) {
            return nullptr;
        }
        return it->second.get();
    }
    bool hasFunction(const std::string& name) const {
        return functions.find(name) != functions.end();
    }
    
    size_t getScopeDepth() const {
        return variableScopes.size();
    }
    void dumpFunctions() const {
        std::cout << "Функции:\n";
        for (const auto& [name, func] : functions) {
            std::cout << "  " << name << "\n";
        }
    }
};

#endif