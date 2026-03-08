#ifndef PARAMETER_H
#define PARAMETER_H
#include <string>

enum Type {
    INT,
    VOID
};

class Parameter {
private:
    const Type type;
    const std::string name;
public:
    Parameter(Type type, std::string name) : type(type), name(name) {}
    Type getType() const {
        return type;
    }
    std::string getName() const {
        return name;
    }
    bool isCompatibleWith(Type argType) const {
        return type == argType;
    }
};

#endif