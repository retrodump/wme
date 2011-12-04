// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeScriptParser_H__
#define __WmeScriptParser_H__

#include <list>
#include <vector>
#include "ErrorLog.h"
#include "WmeScriptLexer.h"

namespace Wme
{
namespace Compiler
{

	class FunctionBodyNode;
	class GCObject;

	class WmeDllExport ScriptParser
	{
	private:
		class WmeDllExport InputContext
		{
		public:
			YY_BUFFER_STATE OrigState;
			int OrigLine;
			WideString OrigFile;
			bool OrigUtf8Mode;
			byte* Buffer;
			dword BufferSize;
			void* FileHandle;  
		};

	public:
		static ScriptParser& GetInstance();

		ScriptParser();
		virtual ~ScriptParser();


		FunctionBodyNode* Parse(const WideString& fileName, ErrorLog* log);
		void Cleanup();
		
		void AddError(const char* format, ...);

		bool OpenFile(const WideString& fileName, byte** outBuffer, dword* outBufferSize, void** outFileHandle);
		bool CloseFile(void* fileHandle);

		const WideString& GetCurrentFile();
		void SetCurrentFile(const WideString& currentFile);

		bool PushInputContext(const AnsiString& newFile, YY_BUFFER_STATE bufferState);
		bool PopInputContext(YY_BUFFER_STATE bufferState);

		void AddGarbage(GCObject* node);

		int GetCurrentLine()
		{
			return m_CurrentLine;
		};

		void IncrementCurrentLine()
		{
			m_CurrentLine++;
		}

		int GetCurrentCol()
		{
			return m_CurrentCol;
		};

		void SetCurrentCol(int col)
		{
			m_CurrentCol = col;

		}

		void IncrementCurrentCol()
		{
			m_CurrentCol++;
		}

		bool GetUtf8Mode()
		{
			return m_Utf8Mode;
		};

		bool GetIgnoreErrors()
		{
			return m_IgnoreErrors;
		};

		void SetIgnoreErrors(bool ignoreErrors)
		{
			m_IgnoreErrors = ignoreErrors;
		};

		FunctionBodyNode* GetRootNode()
		{
			return m_RootNode;
		};

		void SetRootNode(FunctionBodyNode* node)
		{
			m_RootNode = node;
		};

		void EnableParserDebugOutput();

	private:
		int m_CurrentLine;	
		int m_CurrentCol;	
		bool m_IgnoreErrors;
		bool m_Utf8Mode;

		FunctionBodyNode* m_RootNode;
		ErrorLog* m_Log;
		WideString m_CurrentFile;

		typedef std::list<GCObject*> Nodes;
		Nodes m_Nodes;

		typedef std::vector<InputContext*> ContextStack;
		ContextStack m_ContextStack;

		unsigned int m_MaxContextNest;
	};

	int yyerror(char* Msg);
	int yyparse();

#ifdef _DEBUG
	extern int yydebug;
#endif
}
}	
void yy_switch_to_buffer (YY_BUFFER_STATE new_buffer);


#endif // __WmeScriptParser_H_
