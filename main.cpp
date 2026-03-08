#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <any>
#include <memory>
#include <vector>

#include "generated/Lab6GrammarLexer.h"
#include "generated/Lab6GrammarParser.h"
#include "generated/Lab6GrammarBaseVisitor.h"

#include "src/Context.h"
#include "src/Expression.h"
#include "src/Function.h"
#include "src/Parameter.h"
#include "src/ReturnStatement.h"
#include "src/Statement.h"

using namespace antlr4;

void initBuiltinFunctions(Context& context) {
    context.addFunction("print", std::make_unique<PrintFunction>());
    context.addFunction("abs", std::make_unique<AbsFunction>());
    context.addFunction("min", std::make_unique<MinFunction>());
    context.addFunction("max", std::make_unique<MaxFunction>());
    context.addFunction("pow", std::make_unique<PowFunction>());
    context.addFunction("sqr", std::make_unique<SquareFunction>());
    context.addFunction("fact", std::make_unique<FactorialFunction>());
    context.addFunction("isEven", std::make_unique<IsEvenFunction>());
}

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

class ASTBuilderVisitor : public Lab6GrammarBaseVisitor {
private:
    Context& context;
    std::vector<std::unique_ptr<CallStatement>> globalCalls;

    std::unique_ptr<FunctionCallExpression> buildFunctionCallExpr(Lab6GrammarParser::FunctionCallExprContext *ctx) {
        if (!ctx) return nullptr;
        std::string funcName = ctx->funcName->getText();
        std::vector<std::unique_ptr<Expression>> args;
        if (ctx->args) {
            for (auto argCtx : ctx->args->arguments) {
                auto arg = buildExpression(argCtx);
                if (arg) {
                    args.push_back(std::move(arg));
                }
            }
        }
        return std::make_unique<FunctionCallExpression>(funcName, std::move(args));
    }
    std::unique_ptr<Expression> buildExpression(Lab6GrammarParser::ExpressionContext *ctx) {
        if (auto numCtx = dynamic_cast<Lab6GrammarParser::NumberExprContext*>(ctx)) {
            int value = std::stoi(numCtx->number->getText());
            return std::make_unique<IntExpression>(value);
        } else if (auto varCtx = dynamic_cast<Lab6GrammarParser::VarExprContext*>(ctx)) {
            return std::make_unique<VariableExpression>(varCtx->varName->getText());
        } else if (auto callCtx = dynamic_cast<Lab6GrammarParser::CallExprContext*>(ctx)) {
            auto expr = dynamic_cast<Lab6GrammarParser::FunctionCallExprContext*>(callCtx->call);
            return buildFunctionCallExpr(expr); // возвращаем Expression*, все норм
        } else if (auto binCtx = dynamic_cast<Lab6GrammarParser::BinaryExprContext*>(ctx)) {
            auto left = buildExpression(binCtx->left);
            auto right = buildExpression(binCtx->right);
            char op = binCtx->op->getText()[0];
            return std::make_unique<BinaryExpression>(std::move(left), std::move(right), op);
        } else if (auto unCtx = dynamic_cast<Lab6GrammarParser::UnaryExprContext*>(ctx)) {
            auto exp = buildExpression(unCtx->expression());
            char op = unCtx->op->getText()[0];
            return std::make_unique<UnaryExpr>(std::move(exp), op);
        } else if (auto parenCtx = dynamic_cast<Lab6GrammarParser::ParenExprContext*>(ctx)) {
            return buildExpression(parenCtx->inner);
        }
        return nullptr;
    }
    std::unique_ptr<CallStatement> buildCallStmt(Lab6GrammarParser::FunctionCallStmtContext *ctx) {
        auto callExprCtx = dynamic_cast<Lab6GrammarParser::FunctionCallExprContext*>(ctx->call);
        auto funcCall = buildFunctionCallExpr(callExprCtx);
        return std::make_unique<CallStatement>(
            funcCall->getFunctionName(),
            funcCall->releaseArguments()
        );
    }
    std::unique_ptr<ReturnStatement> buildReturnStmt(Lab6GrammarParser::ReturnStmtContext* ctx) {
        if (!ctx) return nullptr;
        auto expr = buildExpression(ctx->value);
        return std::make_unique<ReturnStatement>(std::move(expr));
    }
public:
    ASTBuilderVisitor(Context& ctx) : context(ctx) {}
    std::vector<std::unique_ptr<CallStatement>> extractGlobalCalls() {
        return std::move(globalCalls);
    }

    std::any visitProgram(Lab6GrammarParser::ProgramContext *ctx) override {
        for (auto element : ctx->programElements) {
            visit(element);
        }
        return nullptr;
    }
    std::any visitFunctionDeclElement(Lab6GrammarParser::FunctionDeclElementContext *ctx) override {
        auto funcCtx = ctx->functionDeclaration();
        auto funcDeclCtx = dynamic_cast<Lab6GrammarParser::FunctionDeclContext*>(funcCtx);
        if (!funcDeclCtx) {
            throw std::runtime_error("Ошибка: неверный тип контекста функции");
        }
        std::string funcName = funcDeclCtx->name->getText();
        Type retType;
        if (dynamic_cast<Lab6GrammarParser::IntTypeContext*>(funcDeclCtx->returnType)) {
            retType = INT;
        } else {
            retType = VOID;
        }
        
        std::vector<Parameter> params;
        if (funcDeclCtx->params) {
            for (auto paramCtx : funcDeclCtx->params->parameters) {
                Type paramType;
                if (dynamic_cast<Lab6GrammarParser::IntTypeContext*>(paramCtx->paramType)) {
                    paramType = INT;
                } else {
                    paramType = VOID;
                }
                params.emplace_back(paramType, paramCtx->paramName->getText());
            }
        }
        auto func = std::make_unique<Function>(retType, params);

        if (funcDeclCtx->body) {
            for (auto stmtCtx : funcDeclCtx->body->statements) {
                if (auto callStmtCtx = dynamic_cast<Lab6GrammarParser::FunctionCallStmtContext*>(stmtCtx)) {
                    auto callStmt = buildCallStmt(callStmtCtx);
                    func->addStatement(std::move(callStmt));
                }
            }
            if (funcDeclCtx->body->returnStmt) {
                auto returnCtx = dynamic_cast<Lab6GrammarParser::ReturnStmtContext*>(funcDeclCtx->body->returnStmt);
                std::unique_ptr<ReturnStatement> retStmt;
                if (returnCtx->value) {
                    auto expr = buildExpression(returnCtx->value);
                    retStmt = std::make_unique<ReturnStatement>(std::move(expr));
                } else {
                    retStmt = std::make_unique<ReturnStatement>(nullptr);
                }
                func->setReturnStatement(std::move(retStmt));
            }
        }

        context.addFunction(funcName, std::move(func));
        return nullptr;
    }
    std::any visitFunctionCallElement(Lab6GrammarParser::FunctionCallElementContext *ctx) override {
        auto callExprCtx = dynamic_cast<Lab6GrammarParser::FunctionCallExprContext*>(ctx->functionCall());
        auto funcCall = buildFunctionCallExpr(callExprCtx);
        globalCalls.push_back(std::make_unique<CallStatement>(
            funcCall->getFunctionName(),
            funcCall->releaseArguments()
        ));
        return nullptr;
    }
    std::any visitVoidType(Lab6GrammarParser::VoidTypeContext *ctx) override {
        return nullptr;
    }
    std::any visitIntType(Lab6GrammarParser::IntTypeContext *ctx) override {
        return nullptr;
    }
    std::any visitParameterList(Lab6GrammarParser::ParameterListContext *ctx) override {
        return nullptr;
    }
    std::any visitParameter(Lab6GrammarParser::ParameterContext *ctx) override {
        return nullptr;
    }
    std::any visitStatementList(Lab6GrammarParser::StatementListContext *ctx) override {
        return nullptr;
    }
    std::any visitFunctionCallStmt(Lab6GrammarParser::FunctionCallStmtContext *ctx) override {
        return nullptr;
    }
    std::any visitReturnStmt(Lab6GrammarParser::ReturnStmtContext *ctx) override {
        return nullptr;
    }
    std::any visitFunctionCallExpr(Lab6GrammarParser::FunctionCallExprContext *ctx) override {
        return nullptr;
    }
    
    std::any visitArgumentList(Lab6GrammarParser::ArgumentListContext *ctx) override {
        return nullptr;
    }
    
    std::any visitNumberExpr(Lab6GrammarParser::NumberExprContext *ctx) override {
        return nullptr;
    }
    
    std::any visitBinaryExpr(Lab6GrammarParser::BinaryExprContext *ctx) override {
        return nullptr;
    }
    
    std::any visitVarExpr(Lab6GrammarParser::VarExprContext *ctx) override {
        return nullptr;
    }
    
    std::any visitCallExpr(Lab6GrammarParser::CallExprContext *ctx) override {
        return nullptr;
    }
    
    std::any visitParenExpr(Lab6GrammarParser::ParenExprContext *ctx) override {
        return nullptr;
    }
};

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
            Context context;
            initBuiltinFunctions(context);
            ASTBuilderVisitor astVisitor(context);
            astVisitor.visit(tree);
            auto globalCalls = astVisitor.extractGlobalCalls();
            std::cout << "  Найдено глобальных вызовов: " << globalCalls.size() << std::endl;
            for (auto& st : globalCalls) {
                st->execute(context);
            }
        } else {
            std::cout << "  Программа содержит ошибки." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}