// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeCodeGen_H__
#define __WmeCodeGen_H__

#include <string>
//#include <xstring>
#include <vector>
#include <set>
#include "WmeScript.h"
#include "WmeTypes.h"
#include "DynamicBuffer.h"


namespace Wme
{
namespace Compiler
{
	class StatementNode;
	class FunctionBodyNode;

	class WmeDllExport CodeGen
	{
	private:
		//////////////////////////////////////////////////////////////////////////
		class WmeDllExport FunctionEntry
		{
		public:
			FunctionEntry(const AnsiString& Name, int Address)
			{
				m_Name = Name;
				m_Address = Address;
			}

			AnsiString m_Name;
			int m_Address;
		};

	public:
		CodeGen();
		virtual ~CodeGen();

		void GetCompiledScript(DynamicBuffer* Buffer);

		void SetFileContext(const WideString& Filename, int Line);

		int GetPosition();
		int SeekToPosition(int NewPos);
		int SeekToEnd();


		int PutByte(byte Val);
		int PutInt(int Val);
		int PutDouble(double Val);
		int PutBytes(byte* Buf, int Size);
		int PutInstruction(ScriptInstruction Inst, const AnsiString& InstName);
		int PutString(const AnsiString& Val, bool Utf8);

		void PushLoop(StatementNode* Loop);
		void PopLoop();
		StatementNode* GetLoop();
		StatementNode* GetLoop(const AnsiString& Label);

		void PushScope(FunctionBodyNode* Scope);
		void PopScope();
		int GetScopeNest();
		FunctionBodyNode* GetScope();
		void AddSymbol(const AnsiString& Name, SymbolType Type);
		void AddGlobalSymbol(const AnsiString& Name, SymbolType Type);
		bool HasSymbol(const AnsiString& Name, SymbolType Type, bool CurrentScopeOnly);

		void AddFunctionEntry(const AnsiString& Name, int Address);
		void AddMethodEntry(const AnsiString& Name, int Address);

	private:
		int m_CurrentLine;
		int m_CurrentFile;

		int m_PrevLine;
		int m_PrevFile;

		int GetStringIndex(Utf8String& string);
		
		DynamicBuffer* m_Bytecode;

		typedef std::vector<WideString> FilenameVector;
		FilenameVector m_Files;

		typedef std::vector<Utf8String> Utf8StringVector;
		Utf8StringVector m_StringTable;
		
		typedef std::vector<StatementNode*> StatementNodes;
		StatementNodes m_CurrentLoops;

		typedef std::vector<FunctionBodyNode*> FunctionBodyNodes;
		FunctionBodyNodes m_ScopeStack;

		typedef std::vector<FunctionEntry*> FunctionEntries;
		FunctionEntries m_FunctionEntries;
		FunctionEntries m_MethodEntries;

		typedef std::set<AnsiString> GlobalSymbols;
		GlobalSymbols m_GlobalVariables;
		GlobalSymbols m_GlobalFunctions;
		GlobalSymbols m_GlobalClasses;
	};

}
}
#endif // __WmeCodeGen_H__
