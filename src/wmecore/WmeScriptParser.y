/*
This file is part of Wintermute Engine
For conditions of distribution and use, see copyright notice in license.txt
*/

%{
#define YYPARSER
#include "Wme.h"
#include "CodeNodes.h"
#include "ScriptParser.h"
#include "WmeScriptLexer.h"

#ifdef _DEBUG
#define YYDEBUG 1
#define YYMAXDEPTH 0
#define YYERROR_VERBOSE 1
#endif


extern int yylex();
extern int yyerror (char* );


namespace Wme
{
namespace Compiler
{

%}

%union {
  int                 ival;
  double              dval;
  char                str[4096];
  char                ident[4096];
  Node                *node;
  StatementNode       *stat;
  ParameterNode       *param;
  FunctionBodyNode    *body;
  FuncDeclNode        *func;
  FunctionBodyNode    *prog;
  AssignExprNode      *init;
  SourceElementsNode  *srcs;
  StatListNode        *slist;
  ArgumentsNode       *args;
  ArgumentListNode    *alist;
  VarDeclNode         *decl;
  VarDeclListNode     *vlist;
  CaseBlockNode       *cblk;
  ClauseListNode      *clist;
  CaseClauseNode      *ccl;
  Operator            op;
  Identifier          *id;
}


%start Program


/* literals */
%token NULL_KW TRUE_KW FALSE_KW
%token STRING NUMBER_INT NUMBER_FLOAT

/* keywords */
%token BREAK_KW CASE_KW DEFAULT_KW FOR_KW NEW_KW VAR_KW GLOBAL_KW CONTINUE_KW
%token FUNCTION_KW METHOD_KW RETURN_KW VOID_KW DELETE_KW
%token IF_KW THIS_KW DO_KW WHILE_KW ELSE_KW IN_KW TYPEOF_KW
%token SWITCH_KW WITH_KW RESERVED_KW

/* punctuators */
%token EQEQ NE                     /* == and != */
%token STREQ STRNEQ                /* === and !== */
%token LE GE                       /* <= and >= */
%token OR_KW AND_KW                /* || and && */
%token PLUSPLUS MINUSMINUS         /* ++ and --  */
%token PLUSEQUAL MINUSEQUAL        /* += and -= */
%token MULTEQUAL DIVEQUAL          /* *= and /= */
%token MODEQUAL                    /* %= */
%token TOKEN_ERROR

/* terminal types */
%token <ival> NUMBER_INT
%token <dval> NUMBER_FLOAT
%token <str>  STRING
%token <ident> IDENT


/* non-terminal types */
%type <node>  Literal PrimaryExpression Expression MemberExpression NewExpression CallExpression
%type <node>  LeftHandSideExpression PostfixExpression UnaryExpression
%type <node>  MultiplicativeExpression AdditiveExpression
%type <node>  RelationalExpression EqualityExpression
%type <node>  LogicalANDExpression LogicalORExpression
%type <node>  ConditionalExpression AssignmentExpression
%type <node>  ExprOpt

%type <stat>  Statement Block
%type <stat>  VariableStatement GlobalStatement EmptyStatement ExpressionStatement
%type <stat>  IfStatement IterationStatement ContinueStatement
%type <stat>  BreakStatement ReturnStatement
%type <stat>  SwitchStatement LabelledStatement
%type <stat>  SourceElement

%type <slist> StatementList
%type <init>  Initializer
%type <func>  FunctionDeclaration
%type <body>  FunctionBody
%type <srcs>  SourceElements
%type <param> FormalParameterList
%type <op>    AssignmentOperator
%type <prog>  Program
%type <args>  Arguments
%type <alist> ArgumentList
%type <vlist> VariableDeclarationList GlobalDeclarationList
%type <decl>  VariableDeclaration GlobalDeclaration
%type <cblk>  CaseBlock
%type <ccl>   CaseClause DefaultClause
%type <clist> CaseClauses  CaseClausesOpt
%type <id>    Identifier
%%

Identifier:
    IDENT                          { $$ = new Identifier($1); }
;

Literal:
    NULL_KW                        { $$ = new NullNode(); }
  | TRUE_KW                        { $$ = new BooleanNode(true); }
  | FALSE_KW                       { $$ = new BooleanNode(false); }
  | NUMBER_INT                     { $$ = new NumberNode($1); }
  | NUMBER_FLOAT                   { $$ = new NumberNode($1); }
  | STRING                         { $$ = new StringNode($1); }
;

PrimaryExpression:
    THIS_KW                        { $$ = new ThisNode(); }
  | Identifier                     { $$ = new ResolveNode($1); }
  | Literal
  | '(' Expression ')'             { $$ = new GroupNode($2); }
;


MemberExpression:
    PrimaryExpression
  | MemberExpression '[' Expression ']' { $$ = new AccessorNode1($1, $3); }
  | MemberExpression '.' Identifier     { $$ = new AccessorNode2($1, $3); }
  | NEW_KW MemberExpression Arguments   { $$ = new NewExprNode($2, $3); }
;

NewExpression:
    MemberExpression
  | NEW_KW NewExpression           { $$ = new NewExprNode($2); }
;

CallExpression:
    MemberExpression Arguments        { $$ = new FunctionCallNode($1, $2); }
  | CallExpression Arguments          { $$ = new FunctionCallNode($1, $2); }
  | CallExpression '[' Expression ']' { $$ = new AccessorNode1($1, $3); }
  | CallExpression '.' Identifier     { $$ = new AccessorNode2($1, $3); }
;

Arguments:
    '(' ')'                        { $$ = new ArgumentsNode(); }
  | '(' ArgumentList ')'           { $$ = new ArgumentsNode($2); }
;

ArgumentList:
    AssignmentExpression            { $$ = new ArgumentListNode($1); }
  | ArgumentList ',' AssignmentExpression { $$ = new ArgumentListNode($1, $3); }
;

LeftHandSideExpression:
    NewExpression
  | CallExpression
;

PostfixExpression:    /* TODO: no line terminator here */
    LeftHandSideExpression
  | LeftHandSideExpression PLUSPLUS      { $$ = new PostfixNode($1, OpPlusPlus); }
  | LeftHandSideExpression MINUSMINUS    { $$ = new PostfixNode($1, OpMinusMinus); }
;

UnaryExpression:
    PostfixExpression
  | VOID_KW UnaryExpression        { $$ = new VoidNode($2); }
  | TYPEOF_KW UnaryExpression      { $$ = new TypeOfNode($2); }
  | PLUSPLUS UnaryExpression       { $$ = new PrefixNode(OpPlusPlus, $2); }
  | MINUSMINUS UnaryExpression     { $$ = new PrefixNode(OpMinusMinus, $2); }
  | '+' UnaryExpression            { $$ = new UnaryPlusNode($2); }
  | '-' UnaryExpression            { $$ = new NegateNode($2); }
  | '!' UnaryExpression            { $$ = new LogicalNotNode($2); }
;

MultiplicativeExpression:
    UnaryExpression
  | MultiplicativeExpression '*' UnaryExpression { $$ = new MultNode($1, $3, MultNode::Multiply); }
  | MultiplicativeExpression '/' UnaryExpression { $$ = new MultNode($1, $3, MultNode::Divide); }
  | MultiplicativeExpression '%' UnaryExpression { $$ = new MultNode($1, $3, MultNode::Modulo); }
;

AdditiveExpression:
    MultiplicativeExpression
  | AdditiveExpression '+' MultiplicativeExpression { $$ = new AddNode($1, $3, AddNode::Add); }
  | AdditiveExpression '-' MultiplicativeExpression { $$ = new AddNode($1, $3, AddNode::Subtract); }
;


RelationalExpression:
    AdditiveExpression
  | RelationalExpression '<' AdditiveExpression
                           { $$ = new RelationalNode($1, OpLess, $3); }
  | RelationalExpression '>' AdditiveExpression
                           { $$ = new RelationalNode($1, OpGreater, $3); }
  | RelationalExpression LE AdditiveExpression
                           { $$ = new RelationalNode($1, OpLessEq, $3); }
  | RelationalExpression GE AdditiveExpression
                           { $$ = new RelationalNode($1, OpGreaterEq, $3); }
  | RelationalExpression IN_KW AdditiveExpression
                           { $$ = new RelationalNode($1, OpIn, $3); }
;

EqualityExpression:
    RelationalExpression
  | EqualityExpression EQEQ   RelationalExpression { $$ = new EqualNode($1, OpEqEq, $3); }
  | EqualityExpression NE     RelationalExpression { $$ = new EqualNode($1, OpNotEq, $3); }
  | EqualityExpression STREQ  RelationalExpression { $$ = new EqualNode($1, OpStrEq, $3); }
  | EqualityExpression STRNEQ RelationalExpression { $$ = new EqualNode($1, OpStrNEq, $3);}
;

/* skipping bitwise operations */

LogicalANDExpression:
    EqualityExpression
  | LogicalANDExpression AND_KW EqualityExpression
                           { $$ = new BinaryLogicalNode($1, OpAnd, $3); }
;

LogicalORExpression:
    LogicalANDExpression
  | LogicalORExpression OR_KW LogicalANDExpression
                           { $$ = new BinaryLogicalNode($1, OpOr, $3); }
;

ConditionalExpression:
    LogicalORExpression
  | LogicalORExpression '?' AssignmentExpression ':' AssignmentExpression
                           { $$ = new ConditionalNode($1, $3, $5); }
;

AssignmentExpression:
    ConditionalExpression
  | LeftHandSideExpression AssignmentOperator AssignmentExpression
                           { $$ = new AssignNode($1, $2, $3);}
;

AssignmentOperator:
    '='                            { $$ = OpEqual; }
  | PLUSEQUAL                      { $$ = OpPlusEq; }
  | MINUSEQUAL                     { $$ = OpMinusEq; }
  | MULTEQUAL                      { $$ = OpMultEq; }
  | DIVEQUAL                       { $$ = OpDivEq; }
  | MODEQUAL                       { $$ = OpModEq; }
;

Expression:
    AssignmentExpression
  | Expression ',' AssignmentExpression        { $$ = new CommaNode($1, $3); }
;

Statement:
    Block
  | VariableStatement
  | GlobalStatement
  | EmptyStatement
  | ExpressionStatement
  | IfStatement
  | IterationStatement
  | ContinueStatement
  | BreakStatement
  | ReturnStatement
  | SwitchStatement
  | LabelledStatement
;

Block:
    '{' '}'                        { $$ = new BlockNode(0); }
  | '{' SourceElements '}'         { $$ = new BlockNode($2); }
;

StatementList:
    Statement                      { $$ = new StatListNode($1); }
  | StatementList Statement        { $$ = new StatListNode($1, $2); }
;

VariableStatement:
    VAR_KW VariableDeclarationList ';' { $$ = new VarStatementNode($2); }
;

VariableDeclarationList:
    VariableDeclaration            { $$ = new VarDeclListNode($1); }
  | VariableDeclarationList ',' VariableDeclaration
                                   { $$ = new VarDeclListNode($1, $3); }
;

VariableDeclaration:
    Identifier                     { $$ = new VarDeclNode($1, 0, VarDeclNode::Variable); }
  | Identifier Initializer         { $$ = new VarDeclNode($1, $2, VarDeclNode::Variable); }
;


GlobalStatement:
    GLOBAL_KW GlobalDeclarationList ';' { $$ = new VarStatementNode($2); }
;

GlobalDeclarationList:
    GlobalDeclaration              { $$ = new VarDeclListNode($1); }
  | GlobalDeclarationList ',' GlobalDeclaration
                                   { $$ = new VarDeclListNode($1, $3); }
;

GlobalDeclaration:
    Identifier                     { $$ = new VarDeclNode($1, 0, VarDeclNode::Global); }
  | Identifier Initializer         { $$ = new VarDeclNode($1, $2, VarDeclNode::Global); }
;

Initializer:
    '=' AssignmentExpression       { $$ = new AssignExprNode($2); }
;

EmptyStatement:
    ';'                            { $$ = new EmptyStatementNode(); }
;

ExpressionStatement:
    Expression ';'                 { $$ = new ExprStatementNode($1); }
;

IfStatement: /* shift/reduce conflict due to dangling else */
    IF_KW '(' Expression ')' Statement
                                   { $$ = new IfNode($3,$5,0); }
  | IF_KW '(' Expression ')' Statement ELSE_KW Statement
                                   { $$ = new IfNode($3,$5,$7); }
;

IterationStatement:
    DO_KW Statement WHILE_KW '(' Expression ')' { $$=new DoWhileNode($2,$5); }
  | WHILE_KW '(' Expression ')' Statement   { $$ = new WhileNode($3,$5); }
  | FOR_KW '(' ExprOpt ';' ExprOpt ';' ExprOpt ')'
            Statement              { $$ = new ForNode($3,$5,$7,$9); }
  | FOR_KW '(' VAR_KW VariableDeclarationList ';' ExprOpt ';' ExprOpt ')'
            Statement              { $$ = new ForNode($4,$6,$8,$10); }
  | FOR_KW '(' LeftHandSideExpression IN_KW Expression ')'
            Statement              { $$ = new ForInNode($3, $5, $7); }
  | FOR_KW '(' VAR_KW Identifier IN_KW Expression ')'
            Statement              { $$ = new ForInNode($4,0,$6,$8); }
  | FOR_KW '(' VAR_KW Identifier Initializer IN_KW Expression ')'
            Statement              { $$ = new ForInNode($4,$5,$7,$9); }
;

ExprOpt:
    /* nothing */                  { $$ = 0; }
  | Expression
;

ContinueStatement:
    CONTINUE_KW ';'                   { $$ = new ContinueNode(); }
  | CONTINUE_KW Identifier ';'        { $$ = new ContinueNode($2); }
;

BreakStatement:
    BREAK_KW ';'                      { $$ = new BreakNode(); }
  | BREAK_KW Identifier ';'           { $$ = new BreakNode($2); }
;

ReturnStatement:
    RETURN_KW ';'                     { $$ = new ReturnNode(0); }
  | RETURN_KW Expression ';'          { $$ = new ReturnNode($2); }
;

SwitchStatement:
    SWITCH_KW '(' Expression ')' CaseBlock  { $$ = new SwitchNode($3, $5); }
;

CaseBlock:
    '{' CaseClausesOpt '}'         { $$ = new CaseBlockNode($2, 0, 0); }
  | '{' CaseClausesOpt DefaultClause CaseClausesOpt '}'
                                   { $$ = new CaseBlockNode($2, $3, $4); }
;

CaseClausesOpt:
    /* nothing */                  { $$ = 0; }
  | CaseClauses
;

CaseClauses:
    CaseClause                     { $$ = new ClauseListNode($1); }
  | CaseClauses CaseClause         { $$ = new ClauseListNode($1, $2); }
;

CaseClause:
    CASE_KW Expression ':'         { $$ = new CaseClauseNode($2); }
  | CASE_KW Expression ':' StatementList    { $$ = new CaseClauseNode($2, $4); }
;

DefaultClause:
    DEFAULT_KW ':'                    { $$ = new CaseClauseNode(0); }
  | DEFAULT_KW ':' StatementList      { $$ = new CaseClauseNode(0, $3); }
;

LabelledStatement:
    Identifier ':' Statement       { $3->AddLabel($1);
                                   $$ = new LabelNode($1, $3); }
;

FunctionDeclaration:
    FUNCTION_KW Identifier '(' ')' FunctionBody    { $$ = new FuncDeclNode($2, $5, FuncDeclNode::Function); }
  | FUNCTION_KW Identifier '(' FormalParameterList ')' FunctionBody
                                   { $$ = new FuncDeclNode($2, $4, $6, FuncDeclNode::Function); }
  | METHOD_KW Identifier '(' ')' FunctionBody    { $$ = new FuncDeclNode($2, $5, FuncDeclNode::Method); }
  | METHOD_KW Identifier '(' FormalParameterList ')' FunctionBody
                                   { $$ = new FuncDeclNode($2, $4, $6, FuncDeclNode::Method); }
;

FormalParameterList:
    Identifier                     { $$ = new ParameterNode($1); }
  | FormalParameterList ',' Identifier { $$ = new ParameterNode($1, $3); }
;

FunctionBody:
    '{' '}'                        { $$ = new FunctionBodyNode(0, false); }
  | '{' SourceElements '}'         { $$ = new FunctionBodyNode($2, false); }
;

Program:
    /* nothing, empty script */      { $$ = new FunctionBodyNode(0, true);
                                     ScriptParser::GetInstance().SetRootNode($$); }
    | SourceElements                 { $$ = new FunctionBodyNode($1, true);
                                     ScriptParser::GetInstance().SetRootNode($$); }
;

SourceElements:
    SourceElement                  { $$ = new SourceElementsNode($1); }
  | SourceElements SourceElement   { $$ = new SourceElementsNode($1, $2); } 
;

SourceElement:
    Statement                      { $$ = $1; }
  | FunctionDeclaration            { $$ = $1; }

  /* swallow erroneous statements when needed */
  | error ';'                      { if (ScriptParser::GetInstance().GetIgnoreErrors())
                                       $$ = new EmptyStatementNode();
                                     else
                                       YYABORT;									
                                   }    
  | error '}'                      { if (ScriptParser::GetInstance().GetIgnoreErrors())
                                       $$ = new EmptyStatementNode();
                                     else
                                       YYABORT;									
                                   }    
;


%%
} // namespace Compiler
} // namespace Wme

