// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "ScriptManager.h"
#include "ScriptableObject.h"
#include "Game.h"
#include "ErrorLog.h"
#include "CodeNodes.h"
#include "CodeGen.h"
#include "StringUtil.h"
#include "ScriptGlobalSymbols.h"
#include "DynamicBuffer.h"
#include "FileManager.h"
#include "File.h"
#include "ScString.h"


using namespace Wme::Compiler;


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
ScriptManager::ScriptManager()
{
	m_GlobalSymbols = new ScriptGlobalSymbols();
	m_GlobalSymbols->Register();

	m_StringWrapper = new ScString();
	m_StringWrapper->Register();
}

//////////////////////////////////////////////////////////////////////////
ScriptManager::~ScriptManager()
{
	SAFE_DELETE(m_StringWrapper);
	SAFE_DELETE(m_GlobalSymbols);
	m_Scripts.clear();
}

//////////////////////////////////////////////////////////////////////////
ScriptPtr ScriptManager::RunScript(const WideString& fileName, ScriptableObject* owner)
{
	DynamicBuffer* code = GetCompiledScript(fileName);
	if (!code) return ScriptPtr();

	ScriptPtr newScript = ScriptPtr(new Script(this, owner));
	if (!newScript->LoadFromBuffer(code))
	{
		delete code;		
		newScript.reset();
		return ScriptPtr();
	}
	
	delete code;
	m_Scripts.push_back(newScript);
	newScript->SetFileName(fileName);

#ifdef WME_SCRIPT_COMPILER_DEBUGGING
	newScript->DumpToFile(fileName + L".dump");
	newScript->GetBytecode()->SeekToStart();
#endif

	return newScript;
}

//////////////////////////////////////////////////////////////////////////
DynamicBuffer* ScriptManager::GetCompiledScript(const WideString& fileName)
{
	// detect already compiled scripts
	File* file = Game::GetInstance()->GetFileMgr()->OpenFile(fileName);
	if (!file) return NULL;

	dword magic1, magic2;
	file->Read(&magic1, sizeof(magic1));
	file->Read(&magic2, sizeof(magic2));

	if (magic1 == WME_SCRIPT_MAGIC_1 && magic2 == WME_SCRIPT_MAGIC_2)
	{
		file->Seek(0, File::SEEK_FROM_BEGIN);
		DynamicBuffer* buffer = new DynamicBuffer;
		buffer->FillFromFile(file);

		Game::GetInstance()->GetFileMgr()->CloseFile(file);		
		return buffer;
	}
	else Game::GetInstance()->GetFileMgr()->CloseFile(file);



	// compile script
	ErrorLog* scriptCompilerLog = new ErrorLog();
	CodeGen* context = NULL;
	DynamicBuffer* code = NULL;
	
	FunctionBodyNode* rootNode = ScriptParser::GetInstance().Parse(fileName, scriptCompilerLog);	
	if (rootNode)
	{
		context = new CodeGen();

		// teach the compiler about global variables and functions
		WideStringListPtr nativeProps = m_GlobalSymbols->GetPropList();
		WideStringListPtr nativeFunctions = m_GlobalSymbols->GetMethodList();
		WideStringListPtr nativeClasses = m_GlobalSymbols->GetClassList();

		WideStringList::const_iterator it;
		for (it = nativeProps->begin(); it != nativeProps->end(); it++)
		{
			context->AddGlobalSymbol(StringUtil::WideToAnsi(*it), Wme::Compiler::Variable);
		}
		for (it = nativeFunctions->begin(); it != nativeFunctions->end(); it++)
		{
			context->AddGlobalSymbol(StringUtil::WideToAnsi(*it), Wme::Compiler::Function);
		}
		for (it = nativeClasses->begin(); it != nativeClasses->end(); it++)
		{
			context->AddGlobalSymbol(StringUtil::WideToAnsi(*it), Wme::Compiler::Class);
		}

		rootNode->GetSymbols(context);
		rootNode->Generate(context);
	}

	if (context && scriptCompilerLog->GetEntries().empty())
	{
		code = new DynamicBuffer();
		context->GetCompiledScript(code);
	}
	else
	{
		Game::GetInstance()->Log(L"Error compiling script '" + fileName + L"'.");

		foreach (ErrorLog::ErrorLogEntry* entry, scriptCompilerLog->GetEntries())
		{
			Game::GetInstance()->Log(StringUtil::AnsiToWide(entry->GetText()), entry->GetFileName(), entry->GetLine());
		}
	}

	// cleanup
	SAFE_DELETE(scriptCompilerLog);
	SAFE_DELETE(context);
	ScriptParser::GetInstance().Cleanup();

	return code;
}

//////////////////////////////////////////////////////////////////////////
void ScriptManager::ProcessScripts()
{
	Scripts::iterator it;

	Scripts toDelete;

	// process scripts
	foreach (ScriptPtr script, m_Scripts)
	{
		script->ResolveWaitingStates();
		while (script->IsRunning()) script->Tick();

		if (script->IsFinished()) toDelete.push_back(script);
	}

	// kill finished scripts
	foreach (ScriptPtr script, toDelete)
	{
		script->StopChildThreads();
		if (script->GetOwnerObject()) script->GetOwnerObject()->RemoveScript(script);
		if (script->GetParentScript()) script->GetParentScript()->RemoveChildThread(script);
		m_Scripts.remove(script);
	}
}

//////////////////////////////////////////////////////////////////////////
void ScriptManager::AddScript(ScriptPtr script)
{
	m_Scripts.push_back(script);
}

//////////////////////////////////////////////////////////////////////////
Value* ScriptManager::GetVariableRef(const WideString& name)
{
	if (m_GlobalVars.HasProperty(name)) return m_GlobalVars.GetProperty(name);
	else if (m_GlobalSymbols->HasNativeProperty(name))
	{
		Value* valNative = Game::GetInstance()->GetValueMgr()->CreateValue();
		Value* valRef = Game::GetInstance()->GetValueMgr()->CreateValue();
		
		valNative->SetValue((ScriptableObject*)m_GlobalSymbols);
		valRef->SetReference(valNative, name);
		
		return valRef;
	}
	else return NULL;
}

//////////////////////////////////////////////////////////////////////////
void ScriptManager::AddVariable(const WideString& name)
{
	if (!m_GlobalVars.HasProperty(name))
	{
		m_GlobalVars.SetProperty(name, NULL);
	}
}

//////////////////////////////////////////////////////////////////////////
void ScriptManager::AddRootsToGrayList(ValueManager* valMgr)
{
	m_GlobalVars.AddPropertiesToGrayList(valMgr);
	
	foreach (ScriptPtr script, m_Scripts)
	{
		script->AddRootsToGrayList(valMgr);
	}
}

//////////////////////////////////////////////////////////////////////////
bool ScriptManager::InvokeGlobalFunction(Script* script, const WideString& name)
{
	if (m_GlobalSymbols->HasNativeMethod(name))
		return m_GlobalSymbols->InvokeNativeMethod(script, name, false);
	else
		return false;
}

//////////////////////////////////////////////////////////////////////////
bool ScriptManager::InvokeConstructor(Script* script, const WideString& className)
{
	return m_GlobalSymbols->InvokeConstructor(script, className);
}


} // namespace Wme
