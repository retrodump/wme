// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeScriptLexer_H__
#define __WmeScriptLexer_H__

// Import some variables
extern char* yytext;

typedef struct yy_buffer_state *YY_BUFFER_STATE;
YY_BUFFER_STATE yy_scan_bytes (const char *bytes, int len);
void yy_delete_buffer (YY_BUFFER_STATE b);

// Function prototype
int yylex ();


#endif // __WmeScriptLexer_H__
