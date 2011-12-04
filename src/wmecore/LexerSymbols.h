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


extern YYSTYPE yylval;
