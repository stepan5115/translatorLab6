grammar Lab6Grammar;

program
    : programElements+=programElement* EOF
    ;

programElement
    : functionDeclaration                 #FunctionDeclElement
    | functionCall ';'                    #FunctionCallElement
    ;

functionDeclaration
    : returnType=type name=ID '(' params=parameterList? ')' 
      '{' body=statementList '}'         #FunctionDecl
    ;

type
    : 'void'                               #VoidType
    | 'int'                                #IntType
    ;

parameterList
    : parameters+=parameter (',' parameters+=parameter)*
    ;

parameter
    : paramType=type paramName=ID
    ;

statementList
    : (statements+=statement+)* (returnStmt=returnStatement)?
    ;

statement
    : call=functionCall ';'                #FunctionCallStmt
    ;

returnStatement
    : 'return' value=expression? ';'       #ReturnStmt
    ;

functionCall
    : funcName=ID '(' args=argumentList? ')' #FunctionCallExpr
    ;

argumentList
    : arguments+=expression (',' arguments+=expression)*
    ;

expression
    : '(' inner=expression ')'                #ParenExpr
    | op=('+' | '-') expr=expression          #UnaryExpr
    | left=expression op=('*' | '/' | '%') right=expression  #BinaryExpr
    | left=expression op=('+' | '-') right=expression  #BinaryExpr
    | call=functionCall                      #CallExpr
    | number=NUMBER                           #NumberExpr
    | varName=ID                              #VarExpr
    ;

// Терминалы
ID
    : [a-zA-Z_][a-zA-Z0-9_]*
    ;

NUMBER
    : [0-9]+
    ;

WS
    : [ \t\r\n]+ -> skip
    ;

LINE_COMMENT
    : '//' ~[\r\n]* -> skip
    ;

BLOCK_COMMENT
    : '/*' .*? '*/' -> skip
    ;