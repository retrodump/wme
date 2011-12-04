
/*  A Bison parser, made from ../src/wmecore/wmescriptparser.y with Bison version GNU Bison version 1.24
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	NULL_KW	258
#define	TRUE_KW	259
#define	FALSE_KW	260
#define	STRING	261
#define	NUMBER_INT	262
#define	NUMBER_FLOAT	263
#define	BREAK_KW	264
#define	CASE_KW	265
#define	DEFAULT_KW	266
#define	FOR_KW	267
#define	NEW_KW	268
#define	VAR_KW	269
#define	GLOBAL_KW	270
#define	CONTINUE_KW	271
#define	FUNCTION_KW	272
#define	METHOD_KW	273
#define	RETURN_KW	274
#define	VOID_KW	275
#define	DELETE_KW	276
#define	IF_KW	277
#define	THIS_KW	278
#define	DO_KW	279
#define	WHILE_KW	280
#define	ELSE_KW	281
#define	IN_KW	282
#define	TYPEOF_KW	283
#define	SWITCH_KW	284
#define	WITH_KW	285
#define	RESERVED_KW	286
#define	EQEQ	287
#define	NE	288
#define	STREQ	289
#define	STRNEQ	290
#define	LE	291
#define	GE	292
#define	OR_KW	293
#define	AND_KW	294
#define	PLUSPLUS	295
#define	MINUSMINUS	296
#define	PLUSEQUAL	297
#define	MINUSEQUAL	298
#define	MULTEQUAL	299
#define	DIVEQUAL	300
#define	MODEQUAL	301
#define	TOKEN_ERROR	302
#define	IDENT	303

#line 6 "../src/wmecore/wmescriptparser.y"

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


#line 31 "../src/wmecore/wmescriptparser.y"
typedef union {
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
} YYSTYPE;

#ifndef YYLTYPE
typedef
  struct yyltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  yyltype;

#define YYLTYPE yyltype
#endif

#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		274
#define	YYFLAG		-32768
#define	YYNTBASE	69

#define YYTRANSLATE(x) ((unsigned)(x) <= 303 ? yytranslate[x] : 121)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    57,     2,     2,     2,    60,     2,     2,    49,
    50,    58,    55,    54,    56,    53,    59,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    64,    68,    61,
    65,    62,    63,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    51,     2,    52,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    66,     2,    67,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
    46,    47,    48
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     4,     6,     8,    10,    12,    14,    16,    18,
    20,    24,    26,    31,    35,    39,    41,    44,    47,    50,
    55,    59,    62,    66,    68,    72,    74,    76,    78,    81,
    84,    86,    89,    92,    95,    98,   101,   104,   107,   109,
   113,   117,   121,   123,   127,   131,   133,   137,   141,   145,
   149,   153,   155,   159,   163,   167,   171,   173,   177,   179,
   183,   185,   191,   193,   197,   199,   201,   203,   205,   207,
   209,   211,   215,   217,   219,   221,   223,   225,   227,   229,
   231,   233,   235,   237,   239,   242,   246,   248,   251,   255,
   257,   261,   263,   266,   270,   272,   276,   278,   281,   284,
   286,   289,   295,   303,   310,   316,   326,   337,   345,   354,
   364,   365,   367,   370,   374,   377,   381,   384,   388,   394,
   398,   404,   405,   407,   409,   412,   416,   421,   424,   428,
   432,   438,   445,   451,   458,   460,   464,   467,   471,   472,
   474,   476,   479,   481,   483,   486
};

static const short yyrhs[] = {    48,
     0,     3,     0,     4,     0,     5,     0,     7,     0,     8,
     0,     6,     0,    23,     0,    69,     0,    70,     0,    49,
    89,    50,     0,    71,     0,    72,    51,    89,    52,     0,
    72,    53,    69,     0,    13,    72,    75,     0,    72,     0,
    13,    73,     0,    72,    75,     0,    74,    75,     0,    74,
    51,    89,    52,     0,    74,    53,    69,     0,    49,    50,
     0,    49,    76,    50,     0,    87,     0,    76,    54,    87,
     0,    73,     0,    74,     0,    77,     0,    77,    40,     0,
    77,    41,     0,    78,     0,    20,    79,     0,    28,    79,
     0,    40,    79,     0,    41,    79,     0,    55,    79,     0,
    56,    79,     0,    57,    79,     0,    79,     0,    80,    58,
    79,     0,    80,    59,    79,     0,    80,    60,    79,     0,
    80,     0,    81,    55,    80,     0,    81,    56,    80,     0,
    81,     0,    82,    61,    81,     0,    82,    62,    81,     0,
    82,    36,    81,     0,    82,    37,    81,     0,    82,    27,
    81,     0,    82,     0,    83,    32,    82,     0,    83,    33,
    82,     0,    83,    34,    82,     0,    83,    35,    82,     0,
    83,     0,    84,    39,    83,     0,    84,     0,    85,    38,
    84,     0,    85,     0,    85,    63,    87,    64,    87,     0,
    86,     0,    77,    88,    87,     0,    65,     0,    42,     0,
    43,     0,    44,     0,    45,     0,    46,     0,    87,     0,
    89,    54,    87,     0,    91,     0,    93,     0,    96,     0,
   100,     0,   101,     0,   102,     0,   103,     0,   105,     0,
   106,     0,   107,     0,   108,     0,   114,     0,    66,    67,
     0,    66,   119,    67,     0,    90,     0,    92,    90,     0,
    14,    94,    68,     0,    95,     0,    94,    54,    95,     0,
    69,     0,    69,    99,     0,    15,    97,    68,     0,    98,
     0,    97,    54,    98,     0,    69,     0,    69,    99,     0,
    65,    87,     0,    68,     0,    89,    68,     0,    22,    49,
    89,    50,    90,     0,    22,    49,    89,    50,    90,    26,
    90,     0,    24,    90,    25,    49,    89,    50,     0,    25,
    49,    89,    50,    90,     0,    12,    49,   104,    68,   104,
    68,   104,    50,    90,     0,    12,    49,    14,    94,    68,
   104,    68,   104,    50,    90,     0,    12,    49,    77,    27,
    89,    50,    90,     0,    12,    49,    14,    69,    27,    89,
    50,    90,     0,    12,    49,    14,    69,    99,    27,    89,
    50,    90,     0,     0,    89,     0,    16,    68,     0,    16,
    69,    68,     0,     9,    68,     0,     9,    69,    68,     0,
    19,    68,     0,    19,    89,    68,     0,    29,    49,    89,
    50,   109,     0,    66,   110,    67,     0,    66,   110,   113,
   110,    67,     0,     0,   111,     0,   112,     0,   111,   112,
     0,    10,    89,    64,     0,    10,    89,    64,    92,     0,
    11,    64,     0,    11,    64,    92,     0,    69,    64,    90,
     0,    17,    69,    49,    50,   117,     0,    17,    69,    49,
   116,    50,   117,     0,    18,    69,    49,    50,   117,     0,
    18,    69,    49,   116,    50,   117,     0,    69,     0,   116,
    54,    69,     0,    66,    67,     0,    66,   119,    67,     0,
     0,   119,     0,   120,     0,   119,   120,     0,    90,     0,
   115,     0,     1,    68,     0,     1,    67,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   122,   126,   128,   129,   130,   131,   132,   135,   137,   138,
   139,   143,   145,   146,   147,   150,   152,   155,   157,   158,
   159,   162,   164,   167,   169,   172,   174,   177,   179,   180,
   183,   185,   186,   187,   188,   189,   190,   191,   194,   196,
   197,   198,   201,   203,   204,   208,   210,   212,   214,   216,
   218,   222,   224,   225,   226,   227,   232,   234,   238,   240,
   244,   246,   250,   252,   256,   258,   259,   260,   261,   262,
   265,   267,   270,   272,   273,   274,   275,   276,   277,   278,
   279,   280,   281,   282,   285,   287,   290,   292,   295,   299,
   301,   305,   307,   311,   315,   317,   321,   323,   326,   330,
   334,   338,   341,   345,   347,   348,   350,   352,   354,   356,
   360,   362,   365,   367,   370,   372,   375,   377,   380,   384,
   386,   390,   392,   395,   397,   400,   402,   405,   407,   410,
   415,   417,   419,   420,   424,   426,   429,   431,   434,   437,
   441,   443,   446,   448,   451,   456
};

static const char * const yytname[] = {   "$","error","$undefined.","NULL_KW",
"TRUE_KW","FALSE_KW","STRING","NUMBER_INT","NUMBER_FLOAT","BREAK_KW","CASE_KW",
"DEFAULT_KW","FOR_KW","NEW_KW","VAR_KW","GLOBAL_KW","CONTINUE_KW","FUNCTION_KW",
"METHOD_KW","RETURN_KW","VOID_KW","DELETE_KW","IF_KW","THIS_KW","DO_KW","WHILE_KW",
"ELSE_KW","IN_KW","TYPEOF_KW","SWITCH_KW","WITH_KW","RESERVED_KW","EQEQ","NE",
"STREQ","STRNEQ","LE","GE","OR_KW","AND_KW","PLUSPLUS","MINUSMINUS","PLUSEQUAL",
"MINUSEQUAL","MULTEQUAL","DIVEQUAL","MODEQUAL","TOKEN_ERROR","IDENT","'('","')'",
"'['","']'","'.'","','","'+'","'-'","'!'","'*'","'/'","'%'","'<'","'>'","'?'",
"':'","'='","'{'","'}'","';'","Identifier","Literal","PrimaryExpression","MemberExpression",
"NewExpression","CallExpression","Arguments","ArgumentList","LeftHandSideExpression",
"PostfixExpression","UnaryExpression","MultiplicativeExpression","AdditiveExpression",
"RelationalExpression","EqualityExpression","LogicalANDExpression","LogicalORExpression",
"ConditionalExpression","AssignmentExpression","AssignmentOperator","Expression",
"Statement","Block","StatementList","VariableStatement","VariableDeclarationList",
"VariableDeclaration","GlobalStatement","GlobalDeclarationList","GlobalDeclaration",
"Initializer","EmptyStatement","ExpressionStatement","IfStatement","IterationStatement",
"ExprOpt","ContinueStatement","BreakStatement","ReturnStatement","SwitchStatement",
"CaseBlock","CaseClausesOpt","CaseClauses","CaseClause","DefaultClause","LabelledStatement",
"FunctionDeclaration","FormalParameterList","FunctionBody","Program","SourceElements",
"SourceElement",""
};
#endif

static const short yyr1[] = {     0,
    69,    70,    70,    70,    70,    70,    70,    71,    71,    71,
    71,    72,    72,    72,    72,    73,    73,    74,    74,    74,
    74,    75,    75,    76,    76,    77,    77,    78,    78,    78,
    79,    79,    79,    79,    79,    79,    79,    79,    80,    80,
    80,    80,    81,    81,    81,    82,    82,    82,    82,    82,
    82,    83,    83,    83,    83,    83,    84,    84,    85,    85,
    86,    86,    87,    87,    88,    88,    88,    88,    88,    88,
    89,    89,    90,    90,    90,    90,    90,    90,    90,    90,
    90,    90,    90,    90,    91,    91,    92,    92,    93,    94,
    94,    95,    95,    96,    97,    97,    98,    98,    99,   100,
   101,   102,   102,   103,   103,   103,   103,   103,   103,   103,
   104,   104,   105,   105,   106,   106,   107,   107,   108,   109,
   109,   110,   110,   111,   111,   112,   112,   113,   113,   114,
   115,   115,   115,   115,   116,   116,   117,   117,   118,   118,
   119,   119,   120,   120,   120,   120
};

static const short yyr2[] = {     0,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     3,     1,     4,     3,     3,     1,     2,     2,     2,     4,
     3,     2,     3,     1,     3,     1,     1,     1,     2,     2,
     1,     2,     2,     2,     2,     2,     2,     2,     1,     3,
     3,     3,     1,     3,     3,     1,     3,     3,     3,     3,
     3,     1,     3,     3,     3,     3,     1,     3,     1,     3,
     1,     5,     1,     3,     1,     1,     1,     1,     1,     1,
     1,     3,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     2,     3,     1,     2,     3,     1,
     3,     1,     2,     3,     1,     3,     1,     2,     2,     1,
     2,     5,     7,     6,     5,     9,    10,     7,     8,     9,
     0,     1,     2,     3,     2,     3,     2,     3,     5,     3,
     5,     0,     1,     1,     2,     3,     4,     2,     3,     3,
     5,     6,     5,     6,     1,     3,     2,     3,     0,     1,
     1,     2,     1,     1,     2,     2
};

static const short yydefact[] = {     0,
     0,     2,     3,     4,     7,     5,     6,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     8,     0,
     0,     0,     0,     0,     0,     1,     0,     0,     0,     0,
     0,   100,     9,    10,    12,    16,    26,    27,    28,    31,
    39,    43,    46,    52,    57,    59,    61,    63,    71,     0,
   143,    73,    74,    75,    76,    77,    78,    79,    80,    81,
    82,    83,    84,   144,     0,   141,   146,   145,   115,     0,
   111,     9,    16,    17,    92,     0,    90,    97,     0,    95,
   113,     0,     0,     0,   117,     0,    28,    32,     0,     0,
     0,    33,     0,    34,    35,     0,    36,    37,    38,    85,
     0,     0,     0,     0,     0,    18,     0,     0,    19,    29,
    30,    66,    67,    68,    69,    70,    65,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   101,   142,   116,     0,
    28,   112,     0,    15,     0,    93,     0,    89,    98,     0,
    94,   114,     0,     0,   118,     0,     0,     0,     0,    11,
    86,   130,    22,     0,    24,     0,    14,     0,    21,    64,
    40,    41,    42,    44,    45,    51,    49,    50,    47,    48,
    53,    54,    55,    56,    58,    60,     0,    72,    92,     0,
     0,   111,    99,    91,    96,     0,   135,     0,     0,     0,
     0,     0,     0,     0,    23,     0,    13,    20,     0,     0,
    93,   111,     0,     0,     0,   131,     0,     0,   133,     0,
   102,     0,   105,   122,   119,    25,    62,     0,     0,     0,
     0,   111,   137,     0,   132,   136,   134,     0,   104,     0,
     0,   123,   124,     0,     0,   111,   108,     0,   138,   103,
     0,     0,   120,   122,   125,   109,     0,     0,     0,   126,
   128,     0,   110,     0,   106,    87,   127,   129,   121,   107,
    88,     0,     0,     0
};

static const short yydefgoto[] = {    72,
    34,    35,    36,    37,    38,   106,   164,    39,    40,    41,
    42,    43,    44,    45,    46,    47,    48,    49,   118,    50,
    51,    52,   267,    53,    76,    77,    54,    79,    80,   146,
    55,    56,    57,    58,   143,    59,    60,    61,    62,   225,
   241,   242,   243,   254,    63,    64,   198,   216,   272,    65,
    66
};

static const short yypact[] = {   269,
   130,-32768,-32768,-32768,-32768,-32768,-32768,   -22,   -19,   161,
   -12,   -12,   -14,   -12,   -12,   704,   804,    20,-32768,   674,
    28,   804,    68,   804,   804,-32768,   804,   804,   804,   804,
   404,-32768,    70,-32768,-32768,    47,-32768,    63,   262,-32768,
-32768,   120,   149,    14,   128,     3,    -6,-32768,-32768,   -35,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,   338,-32768,-32768,-32768,-32768,    75,
   743,-32768,    47,-32768,    84,   -16,-32768,    84,     4,-32768,
-32768,   131,   139,   159,-32768,    10,   166,-32768,   804,   189,
   804,-32768,   804,-32768,-32768,    -7,-32768,-32768,-32768,-32768,
   472,   674,   773,   804,   -12,-32768,   804,   -12,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,   804,   804,   804,
   804,   804,   804,   804,   804,   804,   804,   804,   804,   804,
   804,   804,   804,   804,   804,   804,-32768,-32768,-32768,   -12,
    46,   162,   153,-32768,   804,-32768,   -12,-32768,-32768,   -12,
-32768,-32768,    32,    56,-32768,    45,   168,    91,    92,-32768,
-32768,-32768,-32768,    94,-32768,    55,-32768,   137,-32768,-32768,
-32768,-32768,-32768,   120,   120,   149,   149,   149,   149,   149,
    14,    14,    14,    14,   128,     3,   160,-32768,   -17,    17,
   804,   804,-32768,-32768,-32768,   157,-32768,   101,   157,   102,
   674,   804,   674,   163,-32768,   804,-32768,-32768,   804,   804,
   198,   804,   121,   164,   540,-32768,   157,   -12,-32768,   157,
   201,   122,-32768,   218,-32768,-32768,-32768,   123,   804,   165,
   674,   804,-32768,   608,-32768,-32768,-32768,   674,-32768,   804,
     7,   218,-32768,   674,   136,   804,-32768,   185,-32768,-32768,
    39,   173,-32768,   218,-32768,-32768,   674,   190,   674,   674,
   674,   178,-32768,   674,-32768,-32768,   674,   674,-32768,-32768,
-32768,   247,   248,-32768
};

static const short yypgoto[] = {     0,
-32768,-32768,   241,   242,-32768,   -29,-32768,    -1,-32768,    38,
    89,    11,    64,   125,   119,-32768,-32768,   -96,-32768,   -10,
   -18,-32768,    -5,-32768,   114,   108,-32768,-32768,   112,   -74,
-32768,-32768,-32768,-32768,  -187,-32768,-32768,-32768,-32768,-32768,
     9,-32768,    23,-32768,-32768,-32768,   117,  -164,-32768,   -28,
   -64
};


#define	YYLAST		861


static const short yytable[] = {    33,
   138,    90,   101,   149,   214,    86,   165,    70,   109,   210,
    75,    78,    82,    83,    84,    87,    96,   252,   136,    33,
    87,   170,    87,    87,   230,    26,    87,    87,    87,    71,
    33,   134,   137,    26,   219,    26,   138,   147,   187,   188,
   124,   133,   160,   144,   248,    69,   136,   145,   193,   125,
   126,   148,   235,    81,    88,   237,   135,   150,   258,    92,
   142,    94,    95,   136,    33,    97,    98,    99,    89,   141,
   147,   151,   191,   253,   127,   128,    91,   155,   156,    26,
   158,   196,   159,   162,   212,   110,   111,   112,   113,   114,
   115,   116,   136,   166,   201,   103,   168,   104,   136,   105,
    33,    33,   260,    26,   167,   199,   207,   169,   136,   226,
   117,   103,   227,   107,   211,   108,    93,    87,    87,    87,
    87,    87,    87,    87,    87,    87,    87,    87,    87,    87,
    87,    87,    87,   102,   176,   177,   178,   179,   180,   189,
   203,   204,   139,   205,   136,   136,    75,   206,   145,    78,
   217,   220,   197,   197,   218,   218,   171,   172,   173,   129,
   130,   131,   132,     2,     3,     4,     5,     6,     7,   138,
   231,   239,   244,    10,   136,   136,   136,   119,   120,   121,
   213,   142,   221,    19,   223,   257,   234,   153,   208,   136,
   136,   222,   181,   182,   183,   184,    67,    68,   152,   228,
    33,   142,    33,   122,   123,   110,   111,   154,    26,    27,
   174,   175,   247,   157,    33,   136,   202,   236,   245,   250,
   192,   142,   215,   209,   229,   256,   238,   240,   224,   251,
    33,   232,   246,    33,   259,   142,   261,    33,   263,   264,
   265,   266,   266,    33,   269,   270,   273,   274,   271,   271,
    73,    74,   186,   190,   194,   268,    33,   185,    33,    33,
    33,   195,   262,    33,   255,     0,    33,    33,  -139,     1,
   200,     2,     3,     4,     5,     6,     7,     8,     0,     0,
     9,    10,    11,    12,    13,    14,    15,    16,    17,     0,
    18,    19,    20,    21,     0,     0,    22,    23,     0,     0,
     0,   110,   111,   112,   113,   114,   115,   116,    24,    25,
     0,     0,     0,     0,     0,     0,    26,    27,     0,     0,
     0,     0,     0,    28,    29,    30,   117,     0,     0,     0,
     0,     0,     0,     0,    31,     0,    32,  -140,     1,     0,
     2,     3,     4,     5,     6,     7,     8,     0,     0,     9,
    10,    11,    12,    13,    14,    15,    16,    17,     0,    18,
    19,    20,    21,     0,     0,    22,    23,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    24,    25,     0,
     0,     0,     0,     0,     0,    26,    27,     0,     0,     0,
     0,     0,    28,    29,    30,     0,     0,     0,     0,     0,
     0,     0,     0,    31,     1,    32,     2,     3,     4,     5,
     6,     7,     8,     0,     0,     9,    10,    11,    12,    13,
    14,    15,    16,    17,     0,    18,    19,    20,    21,     0,
     0,    22,    23,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,    24,    25,     0,     0,     0,     0,     0,
     0,    26,    27,     0,     0,     0,     0,     0,    28,    29,
    30,     0,     0,     0,     0,     0,     0,     0,     0,    31,
   100,    32,     1,     0,     2,     3,     4,     5,     6,     7,
     8,     0,     0,     9,    10,    11,    12,    13,    14,    15,
    16,    17,     0,    18,    19,    20,    21,     0,     0,    22,
    23,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    24,    25,     0,     0,     0,     0,     0,     0,    26,
    27,     0,     0,     0,     0,     0,    28,    29,    30,     0,
     0,     0,     0,     0,     0,     0,     0,    31,   161,    32,
     1,     0,     2,     3,     4,     5,     6,     7,     8,     0,
     0,     9,    10,    11,    12,    13,    14,    15,    16,    17,
     0,    18,    19,    20,    21,     0,     0,    22,    23,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,    24,
    25,     0,     0,     0,     0,     0,     0,    26,    27,     0,
     0,     0,     0,     0,    28,    29,    30,     0,     0,     0,
     0,     0,     0,     0,     0,    31,   233,    32,     1,     0,
     2,     3,     4,     5,     6,     7,     8,     0,     0,     9,
    10,    11,    12,    13,    14,    15,    16,    17,     0,    18,
    19,    20,    21,     0,     0,    22,    23,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    24,    25,     0,
     0,     0,     0,     0,     0,    26,    27,     0,     0,     0,
     0,     0,    28,    29,    30,     0,     0,     0,     0,     0,
     0,     0,     0,    31,   249,    32,     2,     3,     4,     5,
     6,     7,     8,     0,     0,     9,    10,    11,    12,    13,
     0,     0,    16,    17,     0,    18,    19,    20,    21,     0,
     0,    22,    23,     0,     0,     0,     2,     3,     4,     5,
     6,     7,     0,    24,    25,     0,    10,     0,     0,     0,
     0,    26,    27,    17,     0,     0,    19,     0,    28,    29,
    30,    22,     0,     0,     0,     0,     0,     0,     0,    31,
     0,    32,     0,    24,    25,     2,     3,     4,     5,     6,
     7,    26,    27,     0,     0,    10,   140,     0,    28,    29,
    30,     0,    17,     0,     0,    19,     0,     0,     0,     0,
    22,    85,     0,     0,     0,     2,     3,     4,     5,     6,
     7,     0,    24,    25,     0,    10,     0,     0,     0,     0,
    26,    27,    17,     0,     0,    19,     0,    28,    29,    30,
    22,     0,     0,     0,     0,     0,     2,     3,     4,     5,
     6,     7,    24,    25,     0,     0,    10,     0,     0,     0,
    26,    27,   163,    17,     0,     0,    19,    28,    29,    30,
     0,    22,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,    24,    25,     0,     0,     0,     0,     0,
     0,    26,    27,     0,     0,     0,     0,     0,    28,    29,
    30
};

static const short yycheck[] = {     0,
    65,    20,    31,    78,   192,    16,   103,     8,    38,    27,
    11,    12,    13,    14,    15,    17,    27,    11,    54,    20,
    22,   118,    24,    25,   212,    48,    28,    29,    30,    49,
    31,    38,    68,    48,   199,    48,   101,    54,   135,   136,
    27,    39,    50,    73,   232,    68,    54,    65,   145,    36,
    37,    68,   217,    68,    17,   220,    63,    54,   246,    22,
    71,    24,    25,    54,    65,    28,    29,    30,    49,    71,
    54,    68,    27,    67,    61,    62,    49,    68,    89,    48,
    91,    50,    93,   102,    68,    40,    41,    42,    43,    44,
    45,    46,    54,   104,    50,    49,   107,    51,    54,    53,
   101,   102,    64,    48,   105,    50,    52,   108,    54,   206,
    65,    49,   209,    51,   189,    53,    49,   119,   120,   121,
   122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
   132,   133,   134,    64,   124,   125,   126,   127,   128,   140,
    50,    50,    68,    50,    54,    54,   147,    54,    65,   150,
    50,    50,   153,   154,    54,    54,   119,   120,   121,    32,
    33,    34,    35,     3,     4,     5,     6,     7,     8,   234,
    50,    50,    50,    13,    54,    54,    54,    58,    59,    60,
   191,   192,   201,    23,   203,    50,   215,    49,    52,    54,
    54,   202,   129,   130,   131,   132,    67,    68,    68,   210,
   201,   212,   203,    55,    56,    40,    41,    49,    48,    49,
   122,   123,   231,    25,   215,    54,    49,   218,   229,   238,
    68,   232,    66,    64,    27,   244,    26,    10,    66,   240,
   231,    68,    68,   234,    50,   246,    64,   238,   257,    50,
   259,   260,   261,   244,    67,   264,     0,     0,   267,   268,
    10,    10,   134,   140,   147,   261,   257,   133,   259,   260,
   261,   150,   254,   264,   242,    -1,   267,   268,     0,     1,
   154,     3,     4,     5,     6,     7,     8,     9,    -1,    -1,
    12,    13,    14,    15,    16,    17,    18,    19,    20,    -1,
    22,    23,    24,    25,    -1,    -1,    28,    29,    -1,    -1,
    -1,    40,    41,    42,    43,    44,    45,    46,    40,    41,
    -1,    -1,    -1,    -1,    -1,    -1,    48,    49,    -1,    -1,
    -1,    -1,    -1,    55,    56,    57,    65,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    66,    -1,    68,     0,     1,    -1,
     3,     4,     5,     6,     7,     8,     9,    -1,    -1,    12,
    13,    14,    15,    16,    17,    18,    19,    20,    -1,    22,
    23,    24,    25,    -1,    -1,    28,    29,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    40,    41,    -1,
    -1,    -1,    -1,    -1,    -1,    48,    49,    -1,    -1,    -1,
    -1,    -1,    55,    56,    57,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    66,     1,    68,     3,     4,     5,     6,
     7,     8,     9,    -1,    -1,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    -1,    22,    23,    24,    25,    -1,
    -1,    28,    29,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    40,    41,    -1,    -1,    -1,    -1,    -1,
    -1,    48,    49,    -1,    -1,    -1,    -1,    -1,    55,    56,
    57,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    66,
    67,    68,     1,    -1,     3,     4,     5,     6,     7,     8,
     9,    -1,    -1,    12,    13,    14,    15,    16,    17,    18,
    19,    20,    -1,    22,    23,    24,    25,    -1,    -1,    28,
    29,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    40,    41,    -1,    -1,    -1,    -1,    -1,    -1,    48,
    49,    -1,    -1,    -1,    -1,    -1,    55,    56,    57,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    66,    67,    68,
     1,    -1,     3,     4,     5,     6,     7,     8,     9,    -1,
    -1,    12,    13,    14,    15,    16,    17,    18,    19,    20,
    -1,    22,    23,    24,    25,    -1,    -1,    28,    29,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    40,
    41,    -1,    -1,    -1,    -1,    -1,    -1,    48,    49,    -1,
    -1,    -1,    -1,    -1,    55,    56,    57,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    66,    67,    68,     1,    -1,
     3,     4,     5,     6,     7,     8,     9,    -1,    -1,    12,
    13,    14,    15,    16,    17,    18,    19,    20,    -1,    22,
    23,    24,    25,    -1,    -1,    28,    29,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    40,    41,    -1,
    -1,    -1,    -1,    -1,    -1,    48,    49,    -1,    -1,    -1,
    -1,    -1,    55,    56,    57,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    66,    67,    68,     3,     4,     5,     6,
     7,     8,     9,    -1,    -1,    12,    13,    14,    15,    16,
    -1,    -1,    19,    20,    -1,    22,    23,    24,    25,    -1,
    -1,    28,    29,    -1,    -1,    -1,     3,     4,     5,     6,
     7,     8,    -1,    40,    41,    -1,    13,    -1,    -1,    -1,
    -1,    48,    49,    20,    -1,    -1,    23,    -1,    55,    56,
    57,    28,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    66,
    -1,    68,    -1,    40,    41,     3,     4,     5,     6,     7,
     8,    48,    49,    -1,    -1,    13,    14,    -1,    55,    56,
    57,    -1,    20,    -1,    -1,    23,    -1,    -1,    -1,    -1,
    28,    68,    -1,    -1,    -1,     3,     4,     5,     6,     7,
     8,    -1,    40,    41,    -1,    13,    -1,    -1,    -1,    -1,
    48,    49,    20,    -1,    -1,    23,    -1,    55,    56,    57,
    28,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,
     7,     8,    40,    41,    -1,    -1,    13,    -1,    -1,    -1,
    48,    49,    50,    20,    -1,    -1,    23,    55,    56,    57,
    -1,    28,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    40,    41,    -1,    -1,    -1,    -1,    -1,
    -1,    48,    49,    -1,    -1,    -1,    -1,    -1,    55,    56,
    57
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "bison.simple"

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
int yyparse (void);
#endif

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(FROM,TO,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (from, to, count)
     char *from;
     char *to;
     int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *from, char *to, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 192 "bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#else
#define YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#endif

int
yyparse(YYPARSE_PARAM)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss1, (char *)yyss, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs1, (char *)yyvs, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls1, (char *)yyls, size * sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
#line 123 "../src/wmecore/wmescriptparser.y"
{ yyval.id = new Identifier(yyvsp[0].ident); ;
    break;}
case 2:
#line 127 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new NullNode(); ;
    break;}
case 3:
#line 128 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new BooleanNode(true); ;
    break;}
case 4:
#line 129 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new BooleanNode(false); ;
    break;}
case 5:
#line 130 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new NumberNode(yyvsp[0].ival); ;
    break;}
case 6:
#line 131 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new NumberNode(yyvsp[0].dval); ;
    break;}
case 7:
#line 132 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new StringNode(yyvsp[0].str); ;
    break;}
case 8:
#line 136 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new ThisNode(); ;
    break;}
case 9:
#line 137 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new ResolveNode(yyvsp[0].id); ;
    break;}
case 11:
#line 139 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new GroupNode(yyvsp[-1].node); ;
    break;}
case 13:
#line 145 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new AccessorNode1(yyvsp[-3].node, yyvsp[-1].node); ;
    break;}
case 14:
#line 146 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new AccessorNode2(yyvsp[-2].node, yyvsp[0].id); ;
    break;}
case 15:
#line 147 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new NewExprNode(yyvsp[-1].node, yyvsp[0].args); ;
    break;}
case 17:
#line 152 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new NewExprNode(yyvsp[0].node); ;
    break;}
case 18:
#line 156 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new FunctionCallNode(yyvsp[-1].node, yyvsp[0].args); ;
    break;}
case 19:
#line 157 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new FunctionCallNode(yyvsp[-1].node, yyvsp[0].args); ;
    break;}
case 20:
#line 158 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new AccessorNode1(yyvsp[-3].node, yyvsp[-1].node); ;
    break;}
case 21:
#line 159 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new AccessorNode2(yyvsp[-2].node, yyvsp[0].id); ;
    break;}
case 22:
#line 163 "../src/wmecore/wmescriptparser.y"
{ yyval.args = new ArgumentsNode(); ;
    break;}
case 23:
#line 164 "../src/wmecore/wmescriptparser.y"
{ yyval.args = new ArgumentsNode(yyvsp[-1].alist); ;
    break;}
case 24:
#line 168 "../src/wmecore/wmescriptparser.y"
{ yyval.alist = new ArgumentListNode(yyvsp[0].node); ;
    break;}
case 25:
#line 169 "../src/wmecore/wmescriptparser.y"
{ yyval.alist = new ArgumentListNode(yyvsp[-2].alist, yyvsp[0].node); ;
    break;}
case 29:
#line 179 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new PostfixNode(yyvsp[-1].node, OpPlusPlus); ;
    break;}
case 30:
#line 180 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new PostfixNode(yyvsp[-1].node, OpMinusMinus); ;
    break;}
case 32:
#line 185 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new VoidNode(yyvsp[0].node); ;
    break;}
case 33:
#line 186 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new TypeOfNode(yyvsp[0].node); ;
    break;}
case 34:
#line 187 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new PrefixNode(OpPlusPlus, yyvsp[0].node); ;
    break;}
case 35:
#line 188 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new PrefixNode(OpMinusMinus, yyvsp[0].node); ;
    break;}
case 36:
#line 189 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new UnaryPlusNode(yyvsp[0].node); ;
    break;}
case 37:
#line 190 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new NegateNode(yyvsp[0].node); ;
    break;}
case 38:
#line 191 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new LogicalNotNode(yyvsp[0].node); ;
    break;}
case 40:
#line 196 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new MultNode(yyvsp[-2].node, yyvsp[0].node, MultNode::Multiply); ;
    break;}
case 41:
#line 197 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new MultNode(yyvsp[-2].node, yyvsp[0].node, MultNode::Divide); ;
    break;}
case 42:
#line 198 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new MultNode(yyvsp[-2].node, yyvsp[0].node, MultNode::Modulo); ;
    break;}
case 44:
#line 203 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new AddNode(yyvsp[-2].node, yyvsp[0].node, AddNode::Add); ;
    break;}
case 45:
#line 204 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new AddNode(yyvsp[-2].node, yyvsp[0].node, AddNode::Subtract); ;
    break;}
case 47:
#line 211 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new RelationalNode(yyvsp[-2].node, OpLess, yyvsp[0].node); ;
    break;}
case 48:
#line 213 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new RelationalNode(yyvsp[-2].node, OpGreater, yyvsp[0].node); ;
    break;}
case 49:
#line 215 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new RelationalNode(yyvsp[-2].node, OpLessEq, yyvsp[0].node); ;
    break;}
case 50:
#line 217 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new RelationalNode(yyvsp[-2].node, OpGreaterEq, yyvsp[0].node); ;
    break;}
case 51:
#line 219 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new RelationalNode(yyvsp[-2].node, OpIn, yyvsp[0].node); ;
    break;}
case 53:
#line 224 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new EqualNode(yyvsp[-2].node, OpEqEq, yyvsp[0].node); ;
    break;}
case 54:
#line 225 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new EqualNode(yyvsp[-2].node, OpNotEq, yyvsp[0].node); ;
    break;}
case 55:
#line 226 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new EqualNode(yyvsp[-2].node, OpStrEq, yyvsp[0].node); ;
    break;}
case 56:
#line 227 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new EqualNode(yyvsp[-2].node, OpStrNEq, yyvsp[0].node);;
    break;}
case 58:
#line 235 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new BinaryLogicalNode(yyvsp[-2].node, OpAnd, yyvsp[0].node); ;
    break;}
case 60:
#line 241 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new BinaryLogicalNode(yyvsp[-2].node, OpOr, yyvsp[0].node); ;
    break;}
case 62:
#line 247 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new ConditionalNode(yyvsp[-4].node, yyvsp[-2].node, yyvsp[0].node); ;
    break;}
case 64:
#line 253 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new AssignNode(yyvsp[-2].node, yyvsp[-1].op, yyvsp[0].node);;
    break;}
case 65:
#line 257 "../src/wmecore/wmescriptparser.y"
{ yyval.op = OpEqual; ;
    break;}
case 66:
#line 258 "../src/wmecore/wmescriptparser.y"
{ yyval.op = OpPlusEq; ;
    break;}
case 67:
#line 259 "../src/wmecore/wmescriptparser.y"
{ yyval.op = OpMinusEq; ;
    break;}
case 68:
#line 260 "../src/wmecore/wmescriptparser.y"
{ yyval.op = OpMultEq; ;
    break;}
case 69:
#line 261 "../src/wmecore/wmescriptparser.y"
{ yyval.op = OpDivEq; ;
    break;}
case 70:
#line 262 "../src/wmecore/wmescriptparser.y"
{ yyval.op = OpModEq; ;
    break;}
case 72:
#line 267 "../src/wmecore/wmescriptparser.y"
{ yyval.node = new CommaNode(yyvsp[-2].node, yyvsp[0].node); ;
    break;}
case 85:
#line 286 "../src/wmecore/wmescriptparser.y"
{ yyval.stat = new BlockNode(0); ;
    break;}
case 86:
#line 287 "../src/wmecore/wmescriptparser.y"
{ yyval.stat = new BlockNode(yyvsp[-1].srcs); ;
    break;}
case 87:
#line 291 "../src/wmecore/wmescriptparser.y"
{ yyval.slist = new StatListNode(yyvsp[0].stat); ;
    break;}
case 88:
#line 292 "../src/wmecore/wmescriptparser.y"
{ yyval.slist = new StatListNode(yyvsp[-1].slist, yyvsp[0].stat); ;
    break;}
case 89:
#line 296 "../src/wmecore/wmescriptparser.y"
{ yyval.stat = new VarStatementNode(yyvsp[-1].vlist); ;
    break;}
case 90:
#line 300 "../src/wmecore/wmescriptparser.y"
{ yyval.vlist = new VarDeclListNode(yyvsp[0].decl); ;
    break;}
case 91:
#line 302 "../src/wmecore/wmescriptparser.y"
{ yyval.vlist = new VarDeclListNode(yyvsp[-2].vlist, yyvsp[0].decl); ;
    break;}
case 92:
#line 306 "../src/wmecore/wmescriptparser.y"
{ yyval.decl = new VarDeclNode(yyvsp[0].id, 0, VarDeclNode::Variable); ;
    break;}
case 93:
#line 307 "../src/wmecore/wmescriptparser.y"
{ yyval.decl = new VarDeclNode(yyvsp[-1].id, yyvsp[0].init, VarDeclNode::Variable); ;
    break;}
case 94:
#line 312 "../src/wmecore/wmescriptparser.y"
{ yyval.stat = new VarStatementNode(yyvsp[-1].vlist); ;
    break;}
case 95:
#line 316 "../src/wmecore/wmescriptparser.y"
{ yyval.vlist = new VarDeclListNode(yyvsp[0].decl); ;
    break;}
case 96:
#line 318 "../src/wmecore/wmescriptparser.y"
{ yyval.vlist = new VarDeclListNode(yyvsp[-2].vlist, yyvsp[0].decl); ;
    break;}
case 97:
#line 322 "../src/wmecore/wmescriptparser.y"
{ yyval.decl = new VarDeclNode(yyvsp[0].id, 0, VarDeclNode::Global); ;
    break;}
case 98:
#line 323 "../src/wmecore/wmescriptparser.y"
{ yyval.decl = new VarDeclNode(yyvsp[-1].id, yyvsp[0].init, VarDeclNode::Global); ;
    break;}
case 99:
#line 327 "../src/wmecore/wmescriptparser.y"
{ yyval.init = new AssignExprNode(yyvsp[0].node); ;
    break;}
case 100:
#line 331 "../src/wmecore/wmescriptparser.y"
{ yyval.stat = new EmptyStatementNode(); ;
    break;}
case 101:
#line 335 "../src/wmecore/wmescriptparser.y"
{ yyval.stat = new ExprStatementNode(yyvsp[-1].node); ;
    break;}
case 102:
#line 340 "../src/wmecore/wmescriptparser.y"
{ yyval.stat = new IfNode(yyvsp[-2].node,yyvsp[0].stat,0); ;
    break;}
case 103:
#line 342 "../src/wmecore/wmescriptparser.y"
{ yyval.stat = new IfNode(yyvsp[-4].node,yyvsp[-2].stat,yyvsp[0].stat); ;
    break;}
case 104:
#line 346 "../src/wmecore/wmescriptparser.y"
{ yyval.stat=new DoWhileNode(yyvsp[-4].stat,yyvsp[-1].node); ;
    break;}
case 105:
#line 347 "../src/wmecore/wmescriptparser.y"
{ yyval.stat = new WhileNode(yyvsp[-2].node,yyvsp[0].stat); ;
    break;}
case 106:
#line 349 "../src/wmecore/wmescriptparser.y"
{ yyval.stat = new ForNode(yyvsp[-6].node,yyvsp[-4].node,yyvsp[-2].node,yyvsp[0].stat); ;
    break;}
case 107:
#line 351 "../src/wmecore/wmescriptparser.y"
{ yyval.stat = new ForNode(yyvsp[-6].vlist,yyvsp[-4].node,yyvsp[-2].node,yyvsp[0].stat); ;
    break;}
case 108:
#line 353 "../src/wmecore/wmescriptparser.y"
{ yyval.stat = new ForInNode(yyvsp[-4].node, yyvsp[-2].node, yyvsp[0].stat); ;
    break;}
case 109:
#line 355 "../src/wmecore/wmescriptparser.y"
{ yyval.stat = new ForInNode(yyvsp[-4].id,0,yyvsp[-2].node,yyvsp[0].stat); ;
    break;}
case 110:
#line 357 "../src/wmecore/wmescriptparser.y"
{ yyval.stat = new ForInNode(yyvsp[-5].id,yyvsp[-4].init,yyvsp[-2].node,yyvsp[0].stat); ;
    break;}
case 111:
#line 361 "../src/wmecore/wmescriptparser.y"
{ yyval.node = 0; ;
    break;}
case 113:
#line 366 "../src/wmecore/wmescriptparser.y"
{ yyval.stat = new ContinueNode(); ;
    break;}
case 114:
#line 367 "../src/wmecore/wmescriptparser.y"
{ yyval.stat = new ContinueNode(yyvsp[-1].id); ;
    break;}
case 115:
#line 371 "../src/wmecore/wmescriptparser.y"
{ yyval.stat = new BreakNode(); ;
    break;}
case 116:
#line 372 "../src/wmecore/wmescriptparser.y"
{ yyval.stat = new BreakNode(yyvsp[-1].id); ;
    break;}
case 117:
#line 376 "../src/wmecore/wmescriptparser.y"
{ yyval.stat = new ReturnNode(0); ;
    break;}
case 118:
#line 377 "../src/wmecore/wmescriptparser.y"
{ yyval.stat = new ReturnNode(yyvsp[-1].node); ;
    break;}
case 119:
#line 381 "../src/wmecore/wmescriptparser.y"
{ yyval.stat = new SwitchNode(yyvsp[-2].node, yyvsp[0].cblk); ;
    break;}
case 120:
#line 385 "../src/wmecore/wmescriptparser.y"
{ yyval.cblk = new CaseBlockNode(yyvsp[-1].clist, 0, 0); ;
    break;}
case 121:
#line 387 "../src/wmecore/wmescriptparser.y"
{ yyval.cblk = new CaseBlockNode(yyvsp[-3].clist, yyvsp[-2].ccl, yyvsp[-1].clist); ;
    break;}
case 122:
#line 391 "../src/wmecore/wmescriptparser.y"
{ yyval.clist = 0; ;
    break;}
case 124:
#line 396 "../src/wmecore/wmescriptparser.y"
{ yyval.clist = new ClauseListNode(yyvsp[0].ccl); ;
    break;}
case 125:
#line 397 "../src/wmecore/wmescriptparser.y"
{ yyval.clist = new ClauseListNode(yyvsp[-1].clist, yyvsp[0].ccl); ;
    break;}
case 126:
#line 401 "../src/wmecore/wmescriptparser.y"
{ yyval.ccl = new CaseClauseNode(yyvsp[-1].node); ;
    break;}
case 127:
#line 402 "../src/wmecore/wmescriptparser.y"
{ yyval.ccl = new CaseClauseNode(yyvsp[-2].node, yyvsp[0].slist); ;
    break;}
case 128:
#line 406 "../src/wmecore/wmescriptparser.y"
{ yyval.ccl = new CaseClauseNode(0); ;
    break;}
case 129:
#line 407 "../src/wmecore/wmescriptparser.y"
{ yyval.ccl = new CaseClauseNode(0, yyvsp[0].slist); ;
    break;}
case 130:
#line 411 "../src/wmecore/wmescriptparser.y"
{ yyvsp[0].stat->AddLabel(yyvsp[-2].id);
                                   yyval.stat = new LabelNode(yyvsp[-2].id, yyvsp[0].stat); ;
    break;}
case 131:
#line 416 "../src/wmecore/wmescriptparser.y"
{ yyval.func = new FuncDeclNode(yyvsp[-3].id, yyvsp[0].body, FuncDeclNode::Function); ;
    break;}
case 132:
#line 418 "../src/wmecore/wmescriptparser.y"
{ yyval.func = new FuncDeclNode(yyvsp[-4].id, yyvsp[-2].param, yyvsp[0].body, FuncDeclNode::Function); ;
    break;}
case 133:
#line 419 "../src/wmecore/wmescriptparser.y"
{ yyval.func = new FuncDeclNode(yyvsp[-3].id, yyvsp[0].body, FuncDeclNode::Method); ;
    break;}
case 134:
#line 421 "../src/wmecore/wmescriptparser.y"
{ yyval.func = new FuncDeclNode(yyvsp[-4].id, yyvsp[-2].param, yyvsp[0].body, FuncDeclNode::Method); ;
    break;}
case 135:
#line 425 "../src/wmecore/wmescriptparser.y"
{ yyval.param = new ParameterNode(yyvsp[0].id); ;
    break;}
case 136:
#line 426 "../src/wmecore/wmescriptparser.y"
{ yyval.param = new ParameterNode(yyvsp[-2].param, yyvsp[0].id); ;
    break;}
case 137:
#line 430 "../src/wmecore/wmescriptparser.y"
{ yyval.body = new FunctionBodyNode(0, false); ;
    break;}
case 138:
#line 431 "../src/wmecore/wmescriptparser.y"
{ yyval.body = new FunctionBodyNode(yyvsp[-1].srcs, false); ;
    break;}
case 139:
#line 435 "../src/wmecore/wmescriptparser.y"
{ yyval.prog = new FunctionBodyNode(0, true);
                                     ScriptParser::GetInstance().SetRootNode(yyval.prog); ;
    break;}
case 140:
#line 437 "../src/wmecore/wmescriptparser.y"
{ yyval.prog = new FunctionBodyNode(yyvsp[0].srcs, true);
                                     ScriptParser::GetInstance().SetRootNode(yyval.prog); ;
    break;}
case 141:
#line 442 "../src/wmecore/wmescriptparser.y"
{ yyval.srcs = new SourceElementsNode(yyvsp[0].stat); ;
    break;}
case 142:
#line 443 "../src/wmecore/wmescriptparser.y"
{ yyval.srcs = new SourceElementsNode(yyvsp[-1].srcs, yyvsp[0].stat); ;
    break;}
case 143:
#line 447 "../src/wmecore/wmescriptparser.y"
{ yyval.stat = yyvsp[0].stat; ;
    break;}
case 144:
#line 448 "../src/wmecore/wmescriptparser.y"
{ yyval.stat = yyvsp[0].func; ;
    break;}
case 145:
#line 451 "../src/wmecore/wmescriptparser.y"
{ if (ScriptParser::GetInstance().GetIgnoreErrors())
                                       yyval.stat = new EmptyStatementNode();
                                     else
                                       YYABORT;									
                                   ;
    break;}
case 146:
#line 456 "../src/wmecore/wmescriptparser.y"
{ if (ScriptParser::GetInstance().GetIgnoreErrors())
                                       yyval.stat = new EmptyStatementNode();
                                     else
                                       YYABORT;									
                                   ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 487 "bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;
}
#line 464 "../src/wmecore/wmescriptparser.y"

} // namespace Compiler
} // namespace Wme

