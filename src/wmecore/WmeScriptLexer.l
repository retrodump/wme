/*
This file is part of Wintermute Engine
For conditions of distribution and use, see copyright notice in license.txt
*/

DIGIT     [0-9]
LETTER    [a-zA-Z_]
HEX       [a-fA-F0-9]
BINARY    [0-1]
SCI		    [Ee][+-]?{DIGIT}+
FL		    (f|F)


%{
#include "Wme.h"
#include "WmeScriptParser.h"
#include "CodeNodes.h"
#include "ScriptParser.h"

using namespace Wme::Compiler;

namespace Wme
{
namespace Compiler
{
#include "LexerSymbols.h"
}
}


#define YY_NEVER_INTERACTIVE 1

void CountPos();

int CommentDepth = 0;

#define MAX_STR_CONST 4096

char StringBuf[MAX_STR_CONST];
char* StringBufPtr;
%}


  /* exclusive startconditions */
%x commentsc incl string ustring

%%

  /*  comment start-condition for recognizing multiline comments */
<commentsc>{

	\r\n|\n				{
							CountPos();
							ScriptParser::GetInstance().IncrementCurrentLine();
						}


	"*/"				{
							CountPos();
							CommentDepth--;
							if (CommentDepth == 0) BEGIN(INITIAL);
						}


	"/*"				{
							CountPos();
							CommentDepth++;
						}


	"//"[^\n]*			{
							CountPos();
							/* Single line comments take precedence over multilines */;
						}


	<<EOF>>				{
							BEGIN(INITIAL);
							ScriptParser::GetInstance().AddError("Unclosed comment.");
							return 0;
						}


	[^*/\n]*			{
							CountPos();
							/* ignore anything thats not a '*' or '/' to increase speed */;
						}


	.					{
							CountPos();
							/* ignore (multiline comments)*/;
						}
}

<incl>{

	[ \t]*				{
							CountPos();
							/* eat the whitespace */
						}


	\"[^\"\n]*\"		{
							CountPos();
							yytext[strlen(yytext)-1]='\0';
							
							if (!ScriptParser::GetInstance().PushInputContext(yytext+1, YY_CURRENT_BUFFER))
							{
								BEGIN(INITIAL);
								return 0;
							}

							BEGIN(INITIAL);
						}


	.					{
							CountPos();
							BEGIN(INITIAL);
							ScriptParser::GetInstance().AddError("Include filename expected");
							return 0;
						}
}


<string>{

	\"					{ 
							CountPos();
							BEGIN(INITIAL);
							*StringBufPtr = '\0';
							strcpy(yylval.str, StringBuf);
							return STRING;
						}


	\r\n|\n				{
							CountPos();
							BEGIN(INITIAL);
							ScriptParser::GetInstance().AddError("Unterminated string constant");
							*StringBufPtr = '\0';
							return STRING;
						}


	\\.					{
							CountPos();
							if (StringBufPtr-StringBuf >= MAX_STR_CONST)
							{
								BEGIN(INITIAL);
								ScriptParser::GetInstance().AddError("String constant too long");
								return 0;
							}
							else
							{
								switch (yytext[1])
								{
								case 'n':
									*StringBufPtr++ = '\n';
									break;
								case 't':
									*StringBufPtr++ = '\t';
									break;
								case '\"':
									*StringBufPtr++ = '\"';
									break;
								case '\\':
									*StringBufPtr++ = '\\';
									break;
								default:
									BEGIN(INITIAL);
									ScriptParser::GetInstance().AddError("Unrecognized escape sequence \\%c", yytext[1]);
									return 0;
								}
							}
						}


  .                    {
							CountPos();
							if (StringBufPtr-StringBuf >= MAX_STR_CONST)
							{
								BEGIN(INITIAL);
								ScriptParser::GetInstance().AddError("String constant too long");
								return 0;
							}
							else *StringBufPtr++ = yytext[0];
						}

}


<ustring>{

	\"					{ 
							CountPos();
							BEGIN(INITIAL);
							*StringBufPtr = '\0';
							strcpy(yylval.str, StringBuf);
							return STRING;
						}

	\"\"				{ 
							CountPos();
							if (StringBufPtr-StringBuf >= MAX_STR_CONST)
							{
								BEGIN(INITIAL);
								ScriptParser::GetInstance().AddError("String constant too long");
								return 0;
							}
							else *StringBufPtr++ = yytext[0];
						}


	\r\n|\n				{
							CountPos();
							BEGIN(INITIAL);
							ScriptParser::GetInstance().AddError("Unterminated string constant");
							*StringBufPtr = '\0';
							return STRING;
						}



  .                    {
							CountPos();
							if (StringBufPtr-StringBuf >= MAX_STR_CONST)
							{
								BEGIN(INITIAL);
								ScriptParser::GetInstance().AddError("String constant too long");
								return 0;
							}
							else *StringBufPtr++ = yytext[0];
						}

}


#include				{
							CountPos();
							BEGIN(incl);
						}

[ \t]+					{
							CountPos();
							/* ignore */;
						}


"/*"					{
							CountPos();
							CommentDepth++;
							BEGIN(commentsc);
						}


\r\n|\n					{
							CountPos();
							ScriptParser::GetInstance().IncrementCurrentLine();
						}


"//"[^\n]*				{
							CountPos();
							/* ignore (singleline comment)*/;
						}


#region[^\n]*         /* ignore */;
#endregion[^\n]*      /* ignore */;


"global"				{ CountPos(); return(GLOBAL_KW);     }
"var"					{ CountPos(); return(VAR_KW);        }
"and"					{ CountPos(); return(AND_KW);        }
"or"					{ CountPos(); return(OR_KW);         }
"if"					{ CountPos(); return(IF_KW);         }
"else"					{ CountPos(); return(ELSE_KW);       }
"while"					{ CountPos(); return(WHILE_KW);      }
"for"					{ CountPos(); return(FOR_KW);        }
"in"					{ CountPos(); return(IN_KW);         }
"do"					{ CountPos(); return(DO_KW);         }
"break"					{ CountPos(); return(BREAK_KW);      }
"continue"				{ CountPos(); return(CONTINUE_KW);   }
"null"					{ CountPos(); return(NULL_KW);       }
"return"				{ CountPos(); return(RETURN_KW);     }
"function"				{ CountPos(); return(FUNCTION_KW);   }
"method"				{ CountPos(); return(METHOD_KW);     }
"new"					{ CountPos(); return(NEW_KW);        }
"true"					{ CountPos(); return(TRUE_KW);       }
"false"					{ CountPos(); return(FALSE_KW);      }
"switch"				{ CountPos(); return(SWITCH_KW);     }
"case"					{ CountPos(); return(CASE_KW);       }
"default"				{ CountPos(); return(DEFAULT_KW);    }
"void"					{ CountPos(); return(VOID_KW);       }
"delete"				{ CountPos(); return(DELETE_KW);     }
"this"					{ CountPos(); return(THIS_KW);       }
"typeof"				{ CountPos(); return(TYPEOF_KW);     }
"with"					{ CountPos(); return(WITH_KW);       }
"reserved"				{ CountPos(); return(RESERVED_KW);   }

{LETTER}({LETTER}|{DIGIT})*	{
								CountPos();
								strcpy(yylval.ident, yytext);
								return(IDENT);
							}
                                                                
 /*0[xX]{HEX}+                      { CountPos(); return(CONSTANT_HEX);       }*/
 /*0[bB]{BINARY}+                   { CountPos(); return(CONSTANT_BINARY);    }*/

{DIGIT}+				{
							CountPos();
							yylval.ival = atoi(yytext);
							return(NUMBER_INT);
						}

{DIGIT}+{SCI}{FL}?		{
							CountPos();
							yylval.dval = atof(yytext);
							return(NUMBER_FLOAT);
						}

{DIGIT}*"."{DIGIT}+({SCI})?{FL}?	{
										CountPos();
										yylval.dval = atof(yytext);
										return(NUMBER_FLOAT);
									}

{DIGIT}+"."{DIGIT}*({SCI})?{FL}?	{
										CountPos();
										yylval.dval = atof(yytext);
										return(NUMBER_FLOAT);
									}

\"						{
							CountPos();
							StringBufPtr = StringBuf;
							BEGIN(string);
						}


@\"						{
							CountPos();
							StringBufPtr = StringBuf;
							BEGIN(ustring);
						}

 /*
\"(\\.|[^\\"])*\"		{ return(STRING);    }

`([^`]|`{2})*`			{ return(STRING);    }
*/

"&&"					{ CountPos(); return(AND_KW);             }
"||"					{ CountPos(); return(OR_KW);              }
"+="					{ CountPos(); return(PLUSEQUAL);          }
"-="					{ CountPos(); return(MINUSEQUAL);         }
"*="					{ CountPos(); return(MULTEQUAL);          }
"/="					{ CountPos(); return(DIVEQUAL);           }
"%="					{ CountPos(); return(MODEQUAL);           }
"++"					{ CountPos(); return(PLUSPLUS);           }
"--"					{ CountPos(); return(MINUSMINUS);         }
"<="					{ CountPos(); return(LE);                 }
">="					{ CountPos(); return(GE);                 }
"=="					{ CountPos(); return(EQEQ);               }
"==="					{ CountPos(); return(STREQ);              }
"!=="					{ CountPos(); return(STRNEQ);             }
"!="					{ CountPos(); return(NE);                 }
";"						{ CountPos(); return(';');                }
"{"						{ CountPos(); return('{');                }
"}"						{ CountPos(); return('}');                }
","						{ CountPos(); return(',');                }
"="						{ CountPos(); return('=');                }
"("						{ CountPos(); return('(');                }
")"						{ CountPos(); return(')');                }
"["						{ CountPos(); return('[');                }
"]"						{ CountPos(); return(']');                }
"."						{ CountPos(); return('.');                }
"!"						{ CountPos(); return('!');                }
"-"						{ CountPos(); return('-');                }
"+"						{ CountPos(); return('+');                }
"*"						{ CountPos(); return('*');                }
"/"						{ CountPos(); return('/');                }
"%"						{ CountPos(); return('%');                }
"<"						{ CountPos(); return('<');                }
">"						{ CountPos(); return('>');                }
"&"						{ CountPos(); return('&');                }
"|"						{ CountPos(); return('|');                }
"^"						{ CountPos(); return('^');                }
"~"						{ CountPos(); return('~');                }
":"						{ CountPos(); return(':');                }
"?"						{ CountPos(); return('?');                }
                                  
[ \t\v\r\n\f]			{ CountPos();                             }


<<EOF>>					{
							CountPos();
							if (!ScriptParser::GetInstance().PopInputContext(YY_CURRENT_BUFFER))
								yyterminate();
						}


.						{ CountPos(); /*BEGIN(INITIAL); return 0;*/        }
%%

int yywrap()
{
	return(1);
}

void CountPos()
{
	int i;

	for (i = 0; yytext[i] != '\0'; i++)
		if (yytext[i] == '\n')
			ScriptParser::GetInstance().SetCurrentCol(1);
		//else if (yytext[i] == '\t')
		//	ScriptParser::GetInstance().m_CurrentCol += 8 - (ScriptParser::GetInstance().m_CurrentCol % 8);
		else
			ScriptParser::GetInstance().IncrementCurrentCol();
}
