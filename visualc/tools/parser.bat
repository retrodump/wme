@echo off

copy tools\bin\bison.simple . >nul
copy tools\bin\bison.hairy  . >nul

tools\bin\bison --defines --verbose -o ../src/wmecore/WmeScriptParser.cpp ../src/wmecore/WmeScriptParser.y

del bison.simple  >nul
del bison.hairy  >nul

if exist ..\src\wmecore\LexerSymbols.h del ..\src\wmecore\LexerSymbols.h

ren ..\src\wmecore\WmeScriptParser.cpp.h LexerSymbols.h
ren ..\src\wmecore\wmescriptparser.cpp WmeScriptParser.cpp
