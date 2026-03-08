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
    : varName=ID                            #VarExpr
    | number=NUMBER                          #NumberExpr
    | call=functionCall                      #CallExpr
    | left=expression op=('+' | '-' | '*' | '/' | '%') right=expression  #BinaryExpr
    | '(' inner=expression ')'                #ParenExpr
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