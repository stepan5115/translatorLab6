#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <any>

#include "generated/Lab6GrammarLexer.h"
#include "generated/Lab6GrammarParser.h"
#include "generated/Lab6GrammarBaseVisitor.h"

using namespace antlr4;

class ErrorCountingVisitor : public Lab6GrammarBaseVisitor  {
private:
    int errorCount = 0;
public:
    std::any visitErrorNode(tree::ErrorNode *node) override {
        errorCount++;
        std::cerr << "Ошибка синтаксиса: " << node->getText() << std::endl;
        return {};
    }
    
    int getErrorCount() const { return errorCount; }
};
/*
* Шпоргалка
* Что можно делать в методах:
 * - ctx->children - получить всех детей узла
 * - ctx->getText() - получить текст, соответствующий правилу
 * - ctx->ИМЯ_МЕТКИ - получить доступ к элементам по меткам (например, ctx->name)
 * - visit(child) - рекурсивно обойти дочерний узел
 * - visitChildren(ctx) - обойти всех детей (стандартное поведение)
 */
class Lab6GrammarBaseVisitor : public Lab6GrammarVisitor {
public:
    virtual std::any visitProgram(Lab6GrammarParser::ProgramContext *ctx) override {
        return visitChildren(ctx);
    }
    virtual std::any visitFunctionDeclElement(Lab6GrammarParser::FunctionDeclElementContext *ctx) override {
        return visitChildren(ctx);
    }
    virtual std::any visitFunctionCallElement(Lab6GrammarParser::FunctionCallElementContext *ctx) override {
        return visitChildren(ctx);
    }
    virtual std::any visitFunctionDecl(Lab6GrammarParser::FunctionDeclContext *ctx) override {
        // Пример:
        // std::string funcName = ctx->name->getText();
        // std::string retType = ctx->returnType->getText();
        // if (ctx->params) visit(ctx->params);
        // if (ctx->body) visit(ctx->body);
        return visitChildren(ctx);
    }
    virtual std::any visitVoidType(Lab6GrammarParser::VoidTypeContext *ctx) override {
        return visitChildren(ctx);
    }
    virtual std::any visitIntType(Lab6GrammarParser::IntTypeContext *ctx) override {
        return visitChildren(ctx);
    }
    virtual std::any visitParameterList(Lab6GrammarParser::ParameterListContext *ctx) override {
        // for (auto param : ctx->parameters) {
        //     visit(param);
        // }
        return visitChildren(ctx);
    }
    virtual std::any visitParameter(Lab6GrammarParser::ParameterContext *ctx) override {
        // std::string paramName = ctx->paramName->getText();
        // visit(ctx->paramType); // или ctx->paramType->getText()
        return visitChildren(ctx);
    }
    virtual std::any visitStatementList(Lab6GrammarParser::StatementListContext *ctx) override {
        // for (auto stmt : ctx->statements) {
        //     visit(stmt);
        // }
        // if (ctx->returnStmt) visit(ctx->returnStmt);
        return visitChildren(ctx);
    }
    virtual std::any visitFunctionCallStmt(Lab6GrammarParser::FunctionCallStmtContext *ctx) override {
        // visit(ctx->call);
        return visitChildren(ctx);
    }
    virtual std::any visitReturnStmt(Lab6GrammarParser::ReturnStmtContext *ctx) override {
        // if (ctx->value) {
        //     visit(ctx->value);
        // }
        return visitChildren(ctx);
    }
    virtual std::any visitFunctionCallExpr(Lab6GrammarParser::FunctionCallExprContext *ctx) override {
        // std::string funcName = ctx->funcName->getText();
        // if (ctx->args) visit(ctx->args);
        return visitChildren(ctx);
    }
    virtual std::any visitArgumentList(Lab6GrammarParser::ArgumentListContext *ctx) override {
        // for (auto arg : ctx->arguments) {
        //     visit(arg);
        // }
        return visitChildren(ctx);
    }
    virtual std::any visitNumberExpr(Lab6GrammarParser::NumberExprContext *ctx) override {
        // int value = std::stoi(ctx->number->getText());
        return visitChildren(ctx);
    }
    virtual std::any visitBinaryExpr(Lab6GrammarParser::BinaryExprContext *ctx) override {
        // visit(ctx->left);
        // std::string op = ctx->op->getText();
        // visit(ctx->right);
        return visitChildren(ctx);
    }
    virtual std::any visitVarExpr(Lab6GrammarParser::VarExprContext *ctx) override {
        // std::string varName = ctx->varName->getText();
        return visitChildren(ctx);
    }
    virtual std::any visitCallExpr(Lab6GrammarParser::CallExprContext *ctx) override {
        // return visit(ctx->call);  // Перенаправляем на обработку вызова функции
        return visitChildren(ctx);
    }
    virtual std::any visitParenExpr(Lab6GrammarParser::ParenExprContext *ctx) override {
        // return visit(ctx->inner);  // Скобки не влияют на значение, просто оборачиваем
        return visitChildren(ctx);
    }
}

int main(int argc, char** argv) {
    try {
        if (argc != 2) {
            std::cerr << "Использование: " << argv[0] << " <файл_с_программой>" << std::endl;
            return 1;
        }
        std::string filename = argv[1];
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Ошибка: не удалось открыть файл " << filename << std::endl;
            return 1;
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string input = buffer.str();
        
        std::cout << "Файл загружен: " << filename << std::endl;
        std::cout << "Размер: " << input.length() << " байт" << std::endl;

        ANTLRInputStream inputStream(input);
        Lab6GrammarLexer lexer(&inputStream);
        CommonTokenStream tokens(&lexer);
        tokens.fill();
        std::cout << "\nНайдено токенов: " << tokens.size() << std::endl;
        std::cout << "Токены:" << std::endl;
        for (auto token : tokens.getTokens()) {
            std::cout << "  " << token->toString() << std::endl;
        }
        Lab6GrammarParser parser(&tokens);
        parser.setErrorHandler(std::make_shared<DefaultErrorStrategy>());
        std::cout << "\nРазбор программы..." << std::endl;
        tree::ParseTree* tree = parser.program();
        ErrorCountingVisitor errorVisitor;
        errorVisitor.visit(tree);
        std::cout << "\nРезультат:" << std::endl;
        std::cout << "  Ошибок: " << errorVisitor.getErrorCount() << std::endl;
        
        if (errorVisitor.getErrorCount() == 0) {
            std::cout << "  Программа синтаксически корректна! Выполняем..." << std::endl;
        } else {
            std::cout << "  Программа содержит ошибки." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}