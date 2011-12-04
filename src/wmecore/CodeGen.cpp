// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "CodeGen.h"
#include "StringUtil.h"
#include "CodeNodes.h"
#include "Uuid.h"


namespace Wme
{
namespace Compiler
{


//////////////////////////////////////////////////////////////////////////
CodeGen::CodeGen()
{
	m_CurrentLine = -1;
	m_CurrentFile = -1;

	m_PrevLine = -1;
	m_PrevFile = -1;

	m_Bytecode = new DynamicBuffer();
}

//////////////////////////////////////////////////////////////////////////
CodeGen::~CodeGen()
{
	SAFE_DELETE(m_Bytecode);

	for (FunctionEntries::iterator it = m_FunctionEntries.begin(); it != m_FunctionEntries.end(); it++)
		delete (*it);

	for (FunctionEntries::iterator it = m_MethodEntries.begin(); it != m_MethodEntries.end(); it++)
		delete (*it);
}

//////////////////////////////////////////////////////////////////////////
void CodeGen::SetFileContext(const WideString& Filename, int Line )
{
	int File = -1;

	WideString FilenameLC = Filename;
	StringUtil::ToLowerCase(FilenameLC);

	for (size_t i = 0; i<m_Files.size(); i++)
	{
		if (m_Files[i]==FilenameLC)
		{
			File = i;
			break;
		}
	}
	if (File < 0)
	{
		m_Files.push_back(FilenameLC);
		File = m_Files.size() - 1;
	}

	if (File != m_CurrentFile || Line != m_CurrentLine)
	{
		m_CurrentFile = File;
		m_CurrentLine = Line;
	}
}

//////////////////////////////////////////////////////////////////////////
int CodeGen::PutBytes(byte* Buf, int Size)
{
	return m_Bytecode->PutBytes(Buf, Size);
}


//////////////////////////////////////////////////////////////////////////
int CodeGen::PutByte(byte Val)
{
	return m_Bytecode->Write(Val);
}

//////////////////////////////////////////////////////////////////////////
int CodeGen::PutInt(int Val)
{
	return m_Bytecode->Write(Val);
}

//////////////////////////////////////////////////////////////////////////
int CodeGen::PutDouble(double Val)
{
	return m_Bytecode->Write(Val);
}

//////////////////////////////////////////////////////////////////////////
int CodeGen::PutInstruction(ScriptInstruction Inst, const AnsiString& InstName)
{
	if (Inst!=SI_CHANGE_CONTEXT)
	{
		if (m_CurrentFile != m_PrevFile || m_CurrentLine != m_PrevLine)
		{
			PutInstruction(ISTR(SI_CHANGE_CONTEXT));
			PutInt(m_CurrentFile);
			PutInt(m_CurrentLine);
			m_PrevFile = m_CurrentFile;
			m_PrevLine = m_CurrentLine;
		}
	}

	word w = (word)Inst;

	return m_Bytecode->PutBytes((byte*)&w, sizeof(w));
}

//////////////////////////////////////////////////////////////////////////
int CodeGen::PutString(const AnsiString& Val, bool Utf8)
{
	Utf8String Str;
	if (Utf8)
	{
		Str = (Utf8String)Val;
	}
	else
	{
		WideString WStr = StringUtil::AnsiToWide(Val);
		Str = StringUtil::WideToUtf8(WStr);
	}	
	return m_Bytecode->Write(GetStringIndex(Str));
}

//////////////////////////////////////////////////////////////////////////
int CodeGen::GetPosition()
{
	return m_Bytecode->GetPosition();
}

//////////////////////////////////////////////////////////////////////////
int CodeGen::SeekToPosition(int NewPos)
{
	return m_Bytecode->SeekToPosition(NewPos);
}

//////////////////////////////////////////////////////////////////////////
int CodeGen::SeekToEnd()
{
	return m_Bytecode->SeekToEnd();
}

//////////////////////////////////////////////////////////////////////////
void CodeGen::PushLoop(StatementNode* Loop)
{
	m_CurrentLoops.push_back(Loop);
}

//////////////////////////////////////////////////////////////////////////
void CodeGen::PopLoop()
{
	m_CurrentLoops.pop_back();
}

//////////////////////////////////////////////////////////////////////////
StatementNode* CodeGen::GetLoop()
{
	if (m_CurrentLoops.empty()) return NULL;
	else return m_CurrentLoops.back();
}

//////////////////////////////////////////////////////////////////////////
StatementNode* CodeGen::GetLoop(const AnsiString& Label)
{
	StatementNodes::reverse_iterator it;
	for (it = m_CurrentLoops.rbegin(); it != m_CurrentLoops.rend(); it++)
	{
		if ((*it)->HasLabel(Label)) return *it;
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
void CodeGen::PushScope(FunctionBodyNode* Scope)
{
	m_ScopeStack.push_back(Scope);
}

//////////////////////////////////////////////////////////////////////////
void CodeGen::PopScope()
{
	m_ScopeStack.pop_back();
}

//////////////////////////////////////////////////////////////////////////
int CodeGen::GetScopeNest()
{
	return m_ScopeStack.size();
}

//////////////////////////////////////////////////////////////////////////
FunctionBodyNode* CodeGen::GetScope()
{
	if (m_ScopeStack.size() > 0) return m_ScopeStack.back();
	else return NULL;
}

//////////////////////////////////////////////////////////////////////////
void CodeGen::AddSymbol(const AnsiString& Name, SymbolType Type)
{
	if (m_ScopeStack.size() > 0) m_ScopeStack.back()->AddSymbol(Name, Type);
}


//////////////////////////////////////////////////////////////////////////
void CodeGen::AddGlobalSymbol(const AnsiString& Name, SymbolType Type)
{
	switch (Type)
	{
	case Variable:
		m_GlobalVariables.insert(Name);
		break;
	case Function:
		m_GlobalFunctions.insert(Name);
		break;
	case Class:
		m_GlobalClasses.insert(Name);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
bool CodeGen::HasSymbol(const AnsiString& Name, SymbolType Type, bool CurrentScopeOnly)
{
	if (CurrentScopeOnly)
	{
		if (m_ScopeStack.size() > 0) return m_ScopeStack.back()->HasSymbol(Name, Type);
		else return false;
	}
	else
	{
		FunctionBodyNodes::reverse_iterator it;
		for (it = m_ScopeStack.rbegin(); it != m_ScopeStack.rend(); it++)
		{
			if ((*it)->HasSymbol(Name, Type)) return true;
		}

		switch (Type)
		{
		case Variable:
			if (m_GlobalVariables.find(Name) != m_GlobalVariables.end()) return true;
			break;
		case Function:
			if (m_GlobalFunctions.find(Name) != m_GlobalFunctions.end()) return true;
			break;
		case Class:
			if (m_GlobalClasses.find(Name) != m_GlobalClasses.end()) return true;
			break;
		}
		return false;
	}
}

//////////////////////////////////////////////////////////////////////////
void CodeGen::AddFunctionEntry(const AnsiString& Name, int Address)
{
	m_FunctionEntries.push_back(new FunctionEntry(Name, Address));
}

//////////////////////////////////////////////////////////////////////////
void CodeGen::AddMethodEntry(const AnsiString& Name, int Address)
{
	m_MethodEntries.push_back(new FunctionEntry(Name, Address));
}

//////////////////////////////////////////////////////////////////////////
void CodeGen::GetCompiledScript(DynamicBuffer* Buffer)
{
	// magic & version
	Buffer->Write((dword)WME_SCRIPT_MAGIC_1);
	Buffer->Write((dword)WME_SCRIPT_MAGIC_2);
	Buffer->Write((dword)WME_SCRIPT_VERSION);

	// UUID (for checking if serialized scripts are up-to-date)
	Uuid id(Uuid::UUID_NEW);
	Buffer->Write(id);

	FunctionEntries::iterator it;

	// function table
	Buffer->Write((int)m_FunctionEntries.size());
	for (it = m_FunctionEntries.begin(); it != m_FunctionEntries.end(); it++)
	{
		Buffer->Write((*it)->m_Name);
		Buffer->Write((*it)->m_Address);
	}

	// method table
	Buffer->Write((int)m_MethodEntries.size());
	for (it = m_MethodEntries.begin(); it != m_MethodEntries.end(); it++)
	{
		Buffer->Write((*it)->m_Name);
		Buffer->Write((*it)->m_Address);
	}

	// bytecode
	Buffer->Write((int)m_Bytecode->GetSize());
	Buffer->PutBytes(m_Bytecode->GetData(), m_Bytecode->GetSize());

	// string table
	Buffer->Write((int)m_StringTable.size());
	for (Utf8StringVector::iterator it = m_StringTable.begin(); it != m_StringTable.end(); it++)
	{
		Buffer->Write(*it);
	}

	// file table
	Buffer->Write((int)m_Files.size());
	for (FilenameVector::iterator it = m_Files.begin(); it != m_Files.end(); it++)
	{
		Buffer->Write(StringUtil::WideToUtf8(*it));
	}
}

//////////////////////////////////////////////////////////////////////////
int CodeGen::GetStringIndex(Utf8String& string)
{
	for (size_t i = 0; i < m_StringTable.size(); i++)
	{
		if (m_StringTable[i] == string) return (int)i;
	}
	m_StringTable.push_back(string);
	return (int)(m_StringTable.size() - 1);
}

} // namespace Compiler
} // namespace Wme
