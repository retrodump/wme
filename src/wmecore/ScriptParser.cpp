// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "ScriptParser.h"
#include "WmeScriptLexer.h"
#include "WmeScriptParser.h"
#include "CodeNodes.h"
#include "StringUtil.h"
#include "Game.h"
#include "FileManager.h"
#include "File.h"


namespace Wme
{
namespace Compiler
{
	
//////////////////////////////////////////////////////////////////////////
ScriptParser::ScriptParser()
{
	m_RootNode = NULL;
	m_Log = NULL;

	m_CurrentLine = m_CurrentCol = 0;
	m_CurrentFile = L"";
	m_Utf8Mode = false;
	
	m_MaxContextNest = 20;

	m_IgnoreErrors = false;	
}

//////////////////////////////////////////////////////////////////////////
ScriptParser::~ScriptParser()
{
	Cleanup();
}

//////////////////////////////////////////////////////////////////////////
ScriptParser& ScriptParser::GetInstance()
{
	static ScriptParser Instance;
	return Instance;
}


//////////////////////////////////////////////////////////////////////////
FunctionBodyNode* ScriptParser::Parse(const WideString& fileName, ErrorLog* log)
{
	//EnableParserDebugOutput();

	// init
	Cleanup();

	m_Log = log;
	m_Log->Cleanup();
	
	m_CurrentLine = 1;
	m_CurrentCol = 1;
	m_CurrentFile = L"";


	// load script file
	byte* code;
	dword length;
	void* fileHandle;
	if (!OpenFile(fileName, &code, &length, &fileHandle))
	{
		m_CurrentFile = L"";
		return NULL;
	}
	else SetCurrentFile(fileName);


	// parse
	int parseError = 1;
	YY_BUFFER_STATE bufState;
	if (StringUtil::IsUtf8BOM(code, length))
	{
		m_Utf8Mode = true;
		bufState = yy_scan_bytes((const char*)code + 3, length - 3);
	}
	else
	{
		m_Utf8Mode = false;
		bufState = yy_scan_bytes((const char*)code, length);
	}

	if (bufState)
	{
		parseError = yyparse();
		yy_delete_buffer(bufState);
	}
	CloseFile(fileHandle);


	if (!parseError)
	{
		return m_RootNode;
	}
	else
	{
		return NULL;
	}
}


//////////////////////////////////////////////////////////////////////////
void ScriptParser::Cleanup()
{
	m_CurrentFile = L"";

	m_RootNode = NULL;

	for (Nodes::iterator it = m_Nodes.begin(); it != m_Nodes.end(); it++)
	{
		delete (*it);
	}
	m_Nodes.clear();


	for (ContextStack::iterator it = m_ContextStack.begin(); it != m_ContextStack.end(); it++)
	{
		delete (*it);
	}
	m_ContextStack.clear();

}

//////////////////////////////////////////////////////////////////////////
void ScriptParser::AddError(const char* format, ...)
{
	va_list va;
	char buffer[32768];

	va_start(va, format);
	vsprintf(buffer, format, va);
	va_end(va);

	m_Log->AddEntry(m_CurrentFile, m_CurrentLine, buffer);
}


//////////////////////////////////////////////////////////////////////////
bool ScriptParser::OpenFile(const WideString& fileName, byte** outBuffer, dword* outBufferSize, void** outFileHandle)
{
	File* file = Game::GetInstance()->GetFileMgr()->OpenFile(fileName);
	if (!file) return false;

	size_t fileSize = file->GetSize();

	byte* buffer = new byte[fileSize + 1];
	memset(buffer, 0, fileSize + 1);
	file->Read(buffer, fileSize);

	Game::GetInstance()->GetFileMgr()->CloseFile(file);

	*outBuffer = buffer;
	*outBufferSize = fileSize + 1;
	*outFileHandle = (void*)buffer;

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ScriptParser::CloseFile(void* fileHandle)
{
	byte* buffer = (byte*)fileHandle;
	if (buffer) delete [] buffer;

	return true;
}

//////////////////////////////////////////////////////////////////////////
const WideString& ScriptParser::GetCurrentFile()
{
	return m_CurrentFile;
}

//////////////////////////////////////////////////////////////////////////
void ScriptParser::SetCurrentFile(const WideString& currentFile)
{
	m_CurrentFile = currentFile;
}

//////////////////////////////////////////////////////////////////////////
void ScriptParser::AddGarbage(GCObject* node)
{
	m_Nodes.push_back(node);
}

//////////////////////////////////////////////////////////////////////////
int yyerror(char* msg)
{
	ScriptParser::GetInstance().AddError(msg);
	return 0;
}

//////////////////////////////////////////////////////////////////////////
bool ScriptParser::PushInputContext(const AnsiString& newFile, YY_BUFFER_STATE bufferState)
{
	if (m_ContextStack.size() >= m_MaxContextNest)
	{
		AddError("Includes nested too deeply (%d)", m_MaxContextNest);
		return false;
	}

	InputContext* currContext = new InputContext();
	currContext->OrigState = bufferState;
	currContext->OrigLine = m_CurrentLine;
	currContext->OrigFile = GetCurrentFile();
	currContext->OrigUtf8Mode = m_Utf8Mode;

	WideString newFileWide;
	if (m_Utf8Mode) newFileWide = StringUtil::Utf8ToWide((Utf8String)newFile);
	else newFileWide = StringUtil::AnsiToWide(newFile);

	bool b = OpenFile(newFileWide, &currContext->Buffer, &currContext->BufferSize, &currContext->FileHandle);
	if (!b)
	{
		AddError("Error opening include file: %s", newFile.c_str());
		delete currContext;
		return false;
	}
	SetCurrentFile(newFileWide);

	if (StringUtil::IsUtf8BOM(currContext->Buffer, currContext->BufferSize))
	{
		m_Utf8Mode = true;
		yy_switch_to_buffer(yy_scan_bytes((const char*)currContext->Buffer + 3, currContext->BufferSize));
	}
	else
	{
		m_Utf8Mode = false;
		yy_switch_to_buffer(yy_scan_bytes((const char*)currContext->Buffer, currContext->BufferSize));
	}
	
	m_CurrentLine = 1;

	m_ContextStack.push_back(currContext);

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ScriptParser::PopInputContext(YY_BUFFER_STATE bufferState)
{
	if (!m_ContextStack.size()) return false;

	InputContext* currContext = m_ContextStack.back();
	
	CloseFile(currContext->FileHandle);
	yy_delete_buffer(bufferState);
	yy_switch_to_buffer(currContext->OrigState);
	m_CurrentLine = currContext->OrigLine;
	SetCurrentFile(currContext->OrigFile.c_str());
	m_Utf8Mode = currContext->OrigUtf8Mode;

	delete currContext;
	m_ContextStack.pop_back();

	return true;
}

//////////////////////////////////////////////////////////////////////////
void ScriptParser::EnableParserDebugOutput()
{
#ifdef _DEBUG
#	ifdef WIN32
	if (AllocConsole())
	{
		freopen("CON", "w", stderr);
	}
#	endif

	yydebug = 1;
#endif
}


} // namespace Compiler
} // namespace Wme
