// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Script.h"
#include "ScriptManager.h"
#include "ScriptableObject.h"
#include "DynamicBuffer.h"
#include "WmeScript.h"
#include "FileContent.h"
#include "StringUtil.h"
#include "Game.h"
#include "ScString.h"
#include "ValueManager.h"
#include <boost/lexical_cast.hpp>
#include "XmlUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
Script::Script(ScriptManager* mgr, ScriptableObject* owner)
{
	m_OwnerMgr = mgr;
	m_OwnerObject = owner;
	m_ParentScript = NULL;
	m_State = SCRIPT_RUNNING;
	m_IsChildThread = false;
	m_IsAsyncChildThread = false;

	m_Bytecode = NULL;

	m_CurrentFile = m_CurrentLine = -1;
	
	m_WaitForObjectHandle = -1;
	m_SleepEndTime = 0;
}


//////////////////////////////////////////////////////////////////////////
Script::Script(Script* parentScript, ScriptableObject* owner, Script* requestingScript, const WideString& methodName, bool async)
{
	m_OwnerMgr = parentScript->GetOwnerMgr();
	m_OwnerObject = owner;
	m_ParentScript = parentScript;
	m_State = SCRIPT_RUNNING;
	m_IsChildThread = true;
	m_IsAsyncChildThread = async;
	m_FileName = parentScript->GetFileName();

	m_Uuid = parentScript->GetUuid();
	m_Bytecode = new DynamicBuffer(*parentScript->GetBytecode());

	m_CurrentFile = m_CurrentLine = -1;

	m_WaitForObjectHandle = -1;
	m_SleepEndTime = 0;

	m_ScopeStack.push_back(ScriptScope(L"", ScriptScope::CONTEXT_GLOBAL));
	int StartIP = GetEntryPointAddress(methodName);
	m_Bytecode->SeekToPosition(StartIP);

	// when invoked from other script, we need to grab calling params
	// in case of event handlers we provide the params explicitly
	if (requestingScript) m_Stack.CopyParamsFrom(requestingScript->GetStack());
}


//////////////////////////////////////////////////////////////////////////
Script::~Script()
{
	StopChildThreads();
	SAFE_DELETE(m_Bytecode);

	m_FunctionTable.clear();
	m_MethodTable.clear();
}

//////////////////////////////////////////////////////////////////////////
void Script::StopChildThreads()
{
	foreach (ScriptPtr script, m_ChildThreads)
	{
		script->DecoupleOwnerAndFinish();
	}
	m_ChildThreads.clear();
}

//////////////////////////////////////////////////////////////////////////
void Script::DecoupleOwnerAndFinish()
{
	StopChildThreads();
	SetState(SCRIPT_FINISHED);
	m_OwnerObject = NULL;
	m_ParentScript = NULL;
}

//////////////////////////////////////////////////////////////////////////
void Script::SetState(ScriptState state)
{
	if (m_State != SCRIPT_ERROR) m_State = state;
}

//////////////////////////////////////////////////////////////////////////
bool Script::IsRunning() const
{
	return (m_State == SCRIPT_RUNNING);
}

//////////////////////////////////////////////////////////////////////////
bool Script::IsFinished() const
{
	return (m_State == SCRIPT_FINISHED || m_State == SCRIPT_ERROR);
}

//////////////////////////////////////////////////////////////////////////
Script* Script::CreateChildThread(Script* requestingScript, const WideString& methodName, ScriptableObject* owner, bool async)
{
	Script* childThread = new Script(this, owner, requestingScript, methodName, async);
	ScriptPtr sharedPtr = ScriptPtr(childThread);

	m_OwnerMgr->AddScript(sharedPtr);
	owner->AddScript(sharedPtr);
	
	if (!async) requestingScript->WaitForScript(sharedPtr);

	m_ChildThreads.push_back(sharedPtr);
	
	return childThread;
}

//////////////////////////////////////////////////////////////////////////
void Script::RemoveChildThread(ScriptPtr childThread)
{
	m_ChildThreads.remove(childThread);
}

//////////////////////////////////////////////////////////////////////////
void Script::Tick()
{
	if (!m_Bytecode)
	{
		SetState(SCRIPT_FINISHED);
		return;
	}


	word w;
	m_Bytecode->GetBytes(reinterpret_cast<byte*>(&w), sizeof(w));
	ScriptInstruction inst = (ScriptInstruction)w;

	switch (inst)
	{
	case SI_NOP:
		break;

	case SI_JUMP:
		{
			int address = m_Bytecode->ReadInt();
			m_Bytecode->SeekToPosition(address);
		}
		break;

	case SI_CHANGE_CONTEXT:
		{
			m_CurrentFile = m_Bytecode->ReadInt();
			m_CurrentLine = m_Bytecode->ReadInt();
			// TODO debugger hook
		}
		break;

	case SI_POP_VAR:
		{
			WideString varName = GetStringFromTable(m_Bytecode->ReadInt());
			Value* varRef = GetVariableRef(varName);
			varRef->SetValue(m_Stack.Pop());
		}
		break;

	case SI_POP:
		m_Stack.Pop();
		break;

	case SI_POP_BY_REF:
		{
			Value* valRef = m_Stack.Pop();
			Value* valToSet = m_Stack.Pop();

			valRef->SetValue(valToSet);
		}
		break;

	case SI_CHECK_PARAMS:
		{
			int requiredNumParams = m_Bytecode->ReadInt();
			m_Stack.CheckParams(requiredNumParams);
		}
		break;

	case SI_PUSH_NULL:
		m_Stack.PushNull();
		break;

	case SI_PUSH_THIS:
		m_Stack.Push(m_OwnerObject);
		break;

	case SI_PUSH_VAR_REF:
		{
			WideString varName = GetStringFromTable(m_Bytecode->ReadInt());
			Value* varRef = GetVariableRef(varName);
			m_Stack.Push(varRef);
		}
		break;

	case SI_PUSH_INT:
		{
			int val = m_Bytecode->ReadInt();
			m_Stack.Push(val);
		}
		break;

	case SI_PUSH_DOUBLE:
		{
			double val = m_Bytecode->ReadDouble();
			m_Stack.Push(val);
		}
		break;

	case SI_PUSH_BOOL:
		{
			bool val = (m_Bytecode->ReadByte() != 0);
			m_Stack.Push(val);
		}
		break;

	case SI_PUSH_STRING:
		{
			WideString val = GetStringFromTable(m_Bytecode->ReadInt());
			m_Stack.Push(val);
		}
		break;

	case SI_PUSH_PROP_REF:
		{
			Value* valRef = m_Stack.Pop();
			WideString propName = m_Stack.Pop()->GetString();

			Value* propRef = Game::GetInstance()->GetValueMgr()->CreateValue();
			propRef->SetReference(valRef, propName);
			m_Stack.Push(propRef);
		}
		break;

	case SI_RETURN:
		{
			if (IsChildThread() && m_CallStack.empty())
			{
				if (m_IsAsyncChildThread)
					SetState(SCRIPT_FINISHED);
				else
					SetState(SCRIPT_FINISHED_THREAD);

			}
			else
			{
				if (!m_CallStack.empty())
				{
					int retAddress = m_CallStack.top();
					m_CallStack.pop();			
					m_Bytecode->SeekToPosition(retAddress);
				}
				else m_Bytecode->SeekToEnd();
			}
			m_ScopeStack.pop_back();
		}
		break;

	case SI_START_GLOBAL_SCOPE:
		{
			m_ScopeStack.push_back(ScriptScope(L"", ScriptScope::CONTEXT_GLOBAL));
		}
		break;

	case SI_START_FUNCTION_SCOPE:
		{
			WideString name = GetStringFromTable(m_Bytecode->ReadInt());
			m_ScopeStack.push_back(ScriptScope(name, ScriptScope::CONTEXT_FUNCTION));
		}
		break;

	case SI_START_METHOD_SCOPE:
		{
			WideString name = GetStringFromTable(m_Bytecode->ReadInt());
			m_ScopeStack.push_back(ScriptScope(name, ScriptScope::CONTEXT_METHOD));
		}
		break;

	case SI_DEF_VAR:
		{
			WideString name = GetStringFromTable(m_Bytecode->ReadInt());
			AddLocalVariable(name);
		}
		break;


	case SI_DEF_GLOB:
		{
			WideString name = GetStringFromTable(m_Bytecode->ReadInt());
			AddGlobalVariable(name);
		}
		break;

	case SI_DUP_STACK:
		m_Stack.Push(m_Stack.Top());
		break;

	case SI_JUMP_TRUE:
		{
			int address = m_Bytecode->ReadInt();
			if (m_Stack.Pop()->GetBool() == true) m_Bytecode->SeekToPosition(address);
		}
		break;

	case SI_TYPEOF:
		{
			Value* val = m_Stack.Pop();
			m_Stack.Push(WideString(val->GetTypeName()));
		}
		break;

	case SI_INC_POST:
		{
			Value* valRef = m_Stack.Pop();
			Value* val = Game::GetInstance()->GetValueMgr()->CreateValue();
			val->SetValue(valRef);
			m_Stack.Push(val);

			valRef->Increment();
		}
		break;

	case SI_DEC_POST:
		{
			Value* valRef = m_Stack.Pop();
			Value* val = Game::GetInstance()->GetValueMgr()->CreateValue();
			val->SetValue(valRef);
			m_Stack.Push(val);

			valRef->Decrement();
		}
		break;

	case SI_INC_PRE:
		{
			Value* valRef = m_Stack.Pop();
			valRef->Increment();

			Value* val = Game::GetInstance()->GetValueMgr()->CreateValue();
			val->SetValue(valRef);
			m_Stack.Push(val);
		}
		break;

	case SI_DEC_PRE:
		{
			Value* valRef = m_Stack.Pop();
			valRef->Decrement();

			Value* val = Game::GetInstance()->GetValueMgr()->CreateValue();
			val->SetValue(valRef);
			m_Stack.Push(val);
		}
		break;

	case SI_ADD:
		{
			Value* val1 = m_Stack.Pop();
			Value* val2 = m_Stack.Pop();
			
			Value* result = Game::GetInstance()->GetValueMgr()->CreateValue();
			result->SetValue(val1);
			result->Add(val2);

			m_Stack.Push(result);
		}
		break;

	case SI_SUB:
		{
			Value* val1 = m_Stack.Pop();
			Value* val2 = m_Stack.Pop();

			Value* result = Game::GetInstance()->GetValueMgr()->CreateValue();
			result->SetValue(val1);
			result->Sub(val2);

			m_Stack.Push(result);
		}
		break;

	case SI_MUL:
		{
			Value* val1 = m_Stack.Pop();
			Value* val2 = m_Stack.Pop();

			Value* result = Game::GetInstance()->GetValueMgr()->CreateValue();
			result->SetValue(val1);
			result->Mul(val2);

			m_Stack.Push(result);
		}
		break;

	case SI_DIV:
		{
			Value* val1 = m_Stack.Pop();
			Value* val2 = m_Stack.Pop();

			Value* result = Game::GetInstance()->GetValueMgr()->CreateValue();
			result->SetValue(val1);
			result->Sub(val2);

			m_Stack.Push(result);
		}
		break;

	case SI_MOD:
		{
			Value* val1 = m_Stack.Pop();
			Value* val2 = m_Stack.Pop();

			Value* result = Game::GetInstance()->GetValueMgr()->CreateValue();
			result->SetValue(val1);
			result->Modulo(val2);

			m_Stack.Push(result);
		}
		break;

	case SI_NEG:
		{
			Value* val = m_Stack.Pop();

			Value* result = Game::GetInstance()->GetValueMgr()->CreateValue();
			result->SetValue(val);
			result->Neg();

			m_Stack.Push(result);
		}
		break;

	case SI_ADD_REF:
		{
			Value* valRef = m_Stack.Pop();
			Value* val = m_Stack.Pop();

			valRef->Add(val);

			Value* result = Game::GetInstance()->GetValueMgr()->CreateValue();
			result->SetValue(valRef);
			m_Stack.Push(result);
		}
		break;

	case SI_SUB_REF:
		{
			Value* valRef = m_Stack.Pop();
			Value* val = m_Stack.Pop();

			valRef->Sub(val);

			Value* result = Game::GetInstance()->GetValueMgr()->CreateValue();
			result->SetValue(valRef);
			m_Stack.Push(result);
		}
		break;

	case SI_MUL_REF:
		{
			Value* valRef = m_Stack.Pop();
			Value* val = m_Stack.Pop();

			valRef->Mul(val);

			Value* result = Game::GetInstance()->GetValueMgr()->CreateValue();
			result->SetValue(valRef);
			m_Stack.Push(result);
		}
		break;

	case SI_DIV_REF:
		{
			Value* valRef = m_Stack.Pop();
			Value* val = m_Stack.Pop();

			valRef->Div(val);

			Value* result = Game::GetInstance()->GetValueMgr()->CreateValue();
			result->SetValue(valRef);
			m_Stack.Push(result);
		}
		break;

	case SI_MOD_REF:
		{
			Value* valRef = m_Stack.Pop();
			Value* val = m_Stack.Pop();

			valRef->Modulo(val);

			Value* result = Game::GetInstance()->GetValueMgr()->CreateValue();
			result->SetValue(valRef);
			m_Stack.Push(result);
		}
		break;

	case SI_CMP_LESS:
		{
			Value* val1 = m_Stack.Pop();
			Value* val2 = m_Stack.Pop();

			if (val1->Compare(val2) < 0) m_Stack.Push(true);
			else m_Stack.Push(false);
		}
		break;

	case SI_CMP_LESS_EQUAL:
		{
			Value* val1 = m_Stack.Pop();
			Value* val2 = m_Stack.Pop();

			if (val1->Compare(val2) <= 0) m_Stack.Push(true);
			else m_Stack.Push(false);
		}
		break;

	case SI_CMP_GREATER:
		{
			Value* val1 = m_Stack.Pop();
			Value* val2 = m_Stack.Pop();

			if (val1->Compare(val2) > 0) m_Stack.Push(true);
			else m_Stack.Push(false);
		}
		break;

	case SI_CMP_GREATER_EQUAL:
		{
			Value* val1 = m_Stack.Pop();
			Value* val2 = m_Stack.Pop();

			if (val1->Compare(val2) >= 0) m_Stack.Push(true);
			else m_Stack.Push(false);
		}
		break;

	case SI_CMP_EQUAL:
		{
			Value* val1 = m_Stack.Pop();
			Value* val2 = m_Stack.Pop();

			if (val1->Compare(val2) == 0) m_Stack.Push(true);
			else m_Stack.Push(false);
		}
		break;

	case SI_CMP_NOT_EQUAL:
		{
			Value* val1 = m_Stack.Pop();
			Value* val2 = m_Stack.Pop();

			if (val1->Compare(val2) != 0) m_Stack.Push(true);
			else m_Stack.Push(false);
		}
		break;

	case SI_CMP_STRICT_EQUAL:
		{
			Value* val1 = m_Stack.Pop();
			Value* val2 = m_Stack.Pop();

			m_Stack.Push(val1->StrongEquals(val2));
		}
		break;

	case SI_CMP_STRICT_NOT_EQUAL:
		{
			Value* val1 = m_Stack.Pop();
			Value* val2 = m_Stack.Pop();

			m_Stack.Push(!val1->StrongEquals(val2));
		}
		break;

	case SI_IN:
		{
			Value* propName = m_Stack.Pop();
			Value* testObj = m_Stack.Pop();			

			bool hasProperty = testObj->HasProperty(propName->GetString());
			m_Stack.Push(hasProperty);
		}
		break;

	case SI_AND:
		{
			bool val1 = m_Stack.Pop()->GetBool();
			bool val2 = m_Stack.Pop()->GetBool();

			m_Stack.Push(val1 && val2);
		}
		break;

	case SI_OR:
		{
			bool val1 = m_Stack.Pop()->GetBool();
			bool val2 = m_Stack.Pop()->GetBool();

			m_Stack.Push(val1 || val2);
		}
		break;

	case SI_TO_BOOL:
		{
			Value* val = m_Stack.Pop();
			m_Stack.Push(val->GetBool());
		}
		break;

	case SI_CALL_BY_NAME:
		{
			WideString name = m_Stack.Pop()->GetString();
			int address = GetEntryPointAddress(name);
			if (address < 0)
			{
				if (!m_OwnerMgr->InvokeGlobalFunction(this, name))
				{
					RuntimeError(L"Function/method '" + name + L"' cannot be found.");
					m_Stack.KillParams();
					m_Stack.PushNull();
				}
			}
			else
			{
				m_CallStack.push(m_Bytecode->GetPosition());
				m_Bytecode->SeekToPosition(address);
			}
		}
		break;

	case SI_CALL_BY_REF:
		{
			Value* propRef = m_Stack.Pop();
			
			WideString methodName = propRef->GetPropReference();
			Value* methodObject = propRef->GetReference();

			bool success = false;


			// redirect call for string objects
			if (methodObject->GetType() == Value::VAL_STRING)
			{
				ScString* string = Game::GetInstance()->GetScriptMgr()->GetStringWrapper();
				if (string->HasNativeMethod(methodName))
				{
					string->SetString(methodObject->GetString());
					success = string->InvokeNativeMethod(this, methodName, false);
					if (success) methodObject->SetValue(Value::Create(string->GetString()));
				}
			}
			// generic method call
			else
			{
				ScriptableObject* nativeObject = methodObject->GetNative();
				if (!nativeObject)
				{
					RuntimeError(L"Attempting to call method '" + methodName + L"' on a non-existent object.");
					m_Stack.KillParams();
					m_Stack.PushNull();
					break;
				}

				bool onlyNativeMethods = false;
				if (methodObject->GetNative() == m_OwnerObject)
				{
					// if we are already executing a script method with given name
					// for the requested object, we redirect subsequent calls to native method
					ScriptScope scope = m_ScopeStack.back();
					if (scope.IsSameContext(methodName, ScriptScope::CONTEXT_METHOD))
						onlyNativeMethods = true;
				}
			
				if (onlyNativeMethods)
				{
					// try native method first
					if (nativeObject->HasNativeMethod(methodName))
					{
						success = nativeObject->InvokeNativeMethod(this, methodName, false);
					}

					// otherwise try script method
					if (!success && nativeObject->HasScriptMethod(methodName))
					{
						success = nativeObject->InvokeScriptMethod(this, methodName, false);
					}
				}
				else
				{
					// try script method first
					if (nativeObject->HasScriptMethod(methodName))
					{
						success = nativeObject->InvokeScriptMethod(this, methodName, false);
					}

					// otherwise try native method
					if (!success && nativeObject->HasNativeMethod(methodName))
					{
						success = nativeObject->InvokeNativeMethod(this, methodName, false);
					}
				}
			}


			if (!success)
			{
				RuntimeError(L"Unable to call method '" + methodName + L"'. Call ignored.");
				m_Stack.PushNull();
			}
		}
		break;

	case SI_NEW:
		{
			WideString className = m_Stack.Pop()->GetString();
			if (!m_OwnerMgr->InvokeConstructor(this, className))
			{
				RuntimeError(L"Error instantiating object of class '" + className + L"'.");
				m_Stack.KillParams();
				m_Stack.PushNull();
			}
		}
		break;

	/*
			SI_PUSH_ITERATOR, // peek ObjRef, push IteratorID
			SI_ITERATOR_NEXT, // peek IteratorID, peek ObjRef, push PropName (or null)
	*/
	default:
		RuntimeError(L"Invalid instruction.");
		SetState(SCRIPT_ERROR);
		return;
	}


	// no more code to process?
	if (m_Bytecode->IsAtEOF() && IsRunning())
	{
		// main script with methods -> go to limbo and wait for method calls
		if (!IsChildThread() && m_MethodTable.size() > 0) SetState(SCRIPT_IDLE);
		// otherwise just finish
		else SetState(SCRIPT_FINISHED);
	}


}

//////////////////////////////////////////////////////////////////////////
void Script::RuntimeError(const WideString& str)
{
	WideString error = L"Script runtime error: " + str;
	WideString file = L"";

	file = GetFileNameFromTable(m_CurrentFile);

	Game::GetInstance()->Log(error, file, m_CurrentLine);
	Game::GetInstance()->QuickMessage(L"Script runtime error: " + str);
}

//////////////////////////////////////////////////////////////////////////
bool Script::LoadFromBuffer(DynamicBuffer* buffer)
{
	buffer->SeekToStart();

	dword magic1 = buffer->ReadDword();
	dword magic2 = buffer->ReadDword();
	if (magic1 != WME_SCRIPT_MAGIC_1 || magic2 != WME_SCRIPT_MAGIC_2) return false;

	dword version = buffer->ReadDword();
	if (version > WME_SCRIPT_VERSION) return false;

	m_Uuid = buffer->ReadUuid();

	// function table
	int numFunctions = buffer->ReadInt();
	for (int i = 0; i < numFunctions; i++)
	{
		WideString name = buffer->ReadUtf8String();
		int address = buffer->ReadInt();

		m_FunctionTable[name] = address;
	}

	// method table
	int numMethods = buffer->ReadInt();
	for (int i = 0; i < numMethods; i++)
	{
		WideString name = buffer->ReadUtf8String();
		int address = buffer->ReadInt();

		m_MethodTable[name] = address;
	}

	// bytecode
	int bytecodeSize = buffer->ReadInt();
	if (bytecodeSize > 0)
	{
		byte* bytecode = new byte[bytecodeSize];
		buffer->GetBytes(bytecode, bytecodeSize);
		m_Bytecode = new DynamicBuffer(bytecode, bytecodeSize);
		delete [] bytecode;
	}

	// string table
	int numStrings = buffer->ReadInt();
	for (int i = 0; i < numStrings; i++)
	{
		WideString str = buffer->ReadUtf8String();
		m_StringTable.push_back(str);
	}

	// file table
	int numFiles = buffer->ReadInt();
	for (int i = 0; i < numFiles; i++)
	{
		WideString fileName = buffer->ReadUtf8String();
		m_FileTable.push_back(fileName);
	}

	m_Bytecode->SeekToPosition(0);
	m_State = SCRIPT_RUNNING;

	return true;
}

//////////////////////////////////////////////////////////////////////////
void Script::DumpToFile(const WideString& fileName) const
{
	std::ofstream stream;
#ifdef WIN32
	stream.open(fileName.c_str());
#else
	stream.open(StringUtil::WideToAnsi(fileName).c_str());
#endif
	if (!stream.is_open()) return;

	// read script source contents
	std::vector<FileContent*> fileContent;

	for (size_t i = 0; i < m_FileTable.size(); i++)
	{
		fileContent.push_back(new FileContent(m_FileTable[i]));
	}

	// disassemble bytecode
	m_Bytecode->SeekToStart();
	while (!m_Bytecode->IsAtEOF())
	{
		int address = m_Bytecode->GetPosition();

		word w;
		m_Bytecode->GetBytes(reinterpret_cast<byte*>(&w), sizeof(w));
		ScriptInstruction inst = (ScriptInstruction)w;

		if (inst == SI_CHANGE_CONTEXT)
		{
			int file = m_Bytecode->ReadInt();
			int line = m_Bytecode->ReadInt();

			WideString fileName;
			WideString codeLine;
			if (file >= 0 && file < (int)m_FileTable.size())
			{
				fileName = m_FileTable[file];
				codeLine = fileContent[file]->GetLine(line - 1);
			}
			else
			{
				fileName = L"";
				codeLine = L"";
			}
			if (!codeLine.empty() || !fileName.empty())
			{
				if (!codeLine.empty()) stream << std::endl << StringUtil::WideToUtf8(codeLine) << "  ";
				if (!fileName.empty()) stream << "(" << StringUtil::WideToUtf8(fileName) << ", line " << line << ")";
				stream << std::endl << std::endl;
			}
		}


		stream << "    " << std::setw(8) << std::setfill('0') << address;
		stream << ": ";

		switch (inst)
		{
		case SI_NOP:
			stream << "SI_NOP";
			break;

		case SI_JUMP:
			stream << "SI_JUMP";
			stream << " ";
			stream << std::setw(8) << std::setfill('0') << m_Bytecode->ReadInt();
			break;

		case SI_CHANGE_CONTEXT:
			stream << "SI_CHANGE_CONTEXT";
			break;

		case SI_POP_VAR:
			stream << "SI_POP_VAR";
			stream << " ";
			stream << StringUtil::WideToUtf8(GetStringFromTable(m_Bytecode->ReadInt()));
			break;

		case SI_POP:
			stream << "SI_POP";
			break;

		case SI_POP_BY_REF:
			stream << "SI_POP_BY_REF";
			break;

		case SI_CHECK_PARAMS:
			stream << "SI_CHECK_PARAMS";
			stream << " ";
			stream << m_Bytecode->ReadInt();
			break;

		case SI_PUSH_NULL:
			stream << "SI_PUSH_NULL";
			break;

		case SI_PUSH_THIS:
			stream << "SI_PUSH_THIS";
			break;

		case SI_PUSH_VAR_REF:
			stream << "SI_PUSH_VAR_REF";
			stream << " ";
			stream << StringUtil::WideToUtf8(GetStringFromTable(m_Bytecode->ReadInt()));
			break;

		case SI_PUSH_INT:
			stream << "SI_PUSH_INT";
			stream << " ";
			stream << m_Bytecode->ReadInt();
			break;

		case SI_PUSH_DOUBLE:
			stream << "SI_PUSH_DOUBLE";
			stream << " ";
			stream << m_Bytecode->ReadDouble();
			break;

		case SI_PUSH_BOOL:
			stream << "SI_PUSH_BOOL";
			stream << " ";
			stream << (m_Bytecode->ReadByte()==0?"false":"true");
			break;

		case SI_PUSH_STRING:
			stream << "SI_PUSH_STRING";
			stream << " \"";
			stream << StringUtil::WideToUtf8(GetStringFromTable(m_Bytecode->ReadInt()));
			stream << "\"";
			break;

		case SI_PUSH_PROP_REF:
			stream << "SI_PUSH_PROP_REF";
			break;

		case SI_RETURN:
			stream << "SI_RETURN";
			break;

		case SI_DEF_VAR:
			stream << "SI_DEF_VAR";
			stream << " ";
			stream << StringUtil::WideToUtf8(GetStringFromTable(m_Bytecode->ReadInt()));
			break;

		case SI_DEF_GLOB:
			stream << "SI_DEF_GLOB";
			stream << " ";
			stream << StringUtil::WideToUtf8(GetStringFromTable(m_Bytecode->ReadInt()));
			break;

		case SI_DUP_STACK:
			stream << "SI_DUP_STACK";
			break;

		case SI_JUMP_TRUE:
			stream << "SI_JUMP_TRUE";
			stream << " ";
			stream << std::setw(8) << std::setfill('0') << m_Bytecode->ReadInt();
			break;

		case SI_NEW:
			stream << "SI_NEW";
			break;

		case SI_CALL_BY_NAME:
			stream << "SI_CALL_BY_NAME";
			break;

		case SI_CALL_BY_REF:
			stream << "SI_CALL_BY_REF";
			break;

		case SI_INC_POST:
			stream << "SI_INC_POST";
			break;

		case SI_DEC_POST:
			stream << "SI_DEC_POST";
			break;

		case SI_INC_PRE:
			stream << "SI_INC_PRE";
			break;

		case SI_DEC_PRE:
			stream << "SI_DEC_PRE";
			break;

		case SI_TYPEOF:
			stream << "SI_TYPEOF";
			break;

		case SI_ADD:
			stream << "SI_ADD";
			break;

		case SI_SUB:
			stream << "SI_SUB";
			break;

		case SI_MUL:
			stream << "SI_MUL";
			break;

		case SI_DIV:
			stream << "SI_DIV";
			break;

		case SI_MOD:
			stream << "SI_MOD";
			break;

		case SI_NEG:
			stream << "SI_NEG";
			break;

		case SI_ADD_REF:
			stream << "SI_ADD_REF";
			break;

		case SI_SUB_REF:
			stream << "SI_SUB_REF";
			break;

		case SI_MUL_REF:
			stream << "SI_MUL_REF";
			break;

		case SI_DIV_REF:
			stream << "SI_DIV_REF";
			break;

		case SI_MOD_REF:
			stream << "SI_MOD_REF";
			break;

		case SI_CMP_LESS:
			stream << "SI_CMP_LESS";
			break;

		case SI_CMP_GREATER:
			stream << "SI_CMP_GREATER";
			break;

		case SI_CMP_LESS_EQUAL:
			stream << "SI_CMP_LESS_EQUAL";
			break;

		case SI_CMP_GREATER_EQUAL:
			stream << "SI_CMP_GREATER_EQUAL";
			break;

		case SI_IN:
			stream << "SI_IN";
			break;

		case SI_CMP_EQUAL:
			stream << "SI_CMP_EQUAL";
			break;

		case SI_CMP_NOT_EQUAL:
			stream << "SI_CMP_NOT_EQUAL";
			break;

		case SI_CMP_STRICT_EQUAL:
			stream << "SI_CMP_STRICT_EQUAL";
			break;

		case SI_CMP_STRICT_NOT_EQUAL:
			stream << "SI_CMP_STRICT_NOT_EQUAL";
			break;

		case SI_AND:
			stream << "SI_AND";
			break;

		case SI_OR:
			stream << "SI_OR";
			break;

		case SI_PUSH_ITERATOR:
			stream << "SI_PUSH_ITERATOR";
			break;

		case SI_ITERATOR_NEXT:
			stream << "SI_ITERATOR_NEXT";
			break;

		case SI_START_GLOBAL_SCOPE:
			stream << "SI_START_GLOBAL_SCOPE";
			break;

		case SI_START_FUNCTION_SCOPE:
			stream << "SI_START_FUNCTION_SCOPE";
			stream << " ";
			stream << StringUtil::WideToUtf8(GetStringFromTable(m_Bytecode->ReadInt()));
			break;

		case SI_START_METHOD_SCOPE:
			stream << "SI_START_METHOD_SCOPE";
			stream << " ";
			stream << StringUtil::WideToUtf8(GetStringFromTable(m_Bytecode->ReadInt()));
			break;

		case SI_TO_BOOL:
			stream << "SI_TO_BOOL";
			break;

		default:
			stream << "invalid instruction " << inst;
		}

		stream << std::endl;
	}


	stream << std::endl << "----------------------------------------";

	EntryPointMap::const_iterator it;

	// function table
	if (m_FunctionTable.size() > 0)
	{
		stream << std::endl << "Function table:" << std::endl;
		for (it = m_FunctionTable.begin(); it != m_FunctionTable.end(); it++)
		{
			stream << "    " << std::setw(8) << std::setfill('0') << (*it).second;
			stream << ": " << StringUtil::WideToUtf8((*it).first) << std::endl;
		}
	}

	// method table
	if (m_MethodTable.size() > 0)
	{
		stream << std::endl << "Method table:" << std::endl;
		for (it = m_MethodTable.begin(); it != m_MethodTable.end(); it++)
		{
			stream << "    " << std::setw(8) << std::setfill('0') << (*it).second;
			stream << ": " << StringUtil::WideToUtf8((*it).first) << std::endl;
		}
	}

	// uuid
	stream << std::endl << "UUID: " << StringUtil::WideToUtf8(m_Uuid.ToString()) << std::endl;


	stream.close();

	foreach (FileContent* content, fileContent)
	{
		SAFE_DELETE(content);
	}
}

//////////////////////////////////////////////////////////////////////////
WideString Script::GetStringFromTable(int stringID) const
{
	if (IsChildThread())
	{
		return m_ParentScript->GetStringFromTable(stringID);
	}

	if (stringID < (int)m_StringTable.size()) return m_StringTable[stringID];
	else return L"(invalid string)";
}


//////////////////////////////////////////////////////////////////////////
WideString Script::GetFileNameFromTable(int fileID) const
{
	if (IsChildThread())
	{
		return m_ParentScript->GetFileNameFromTable(fileID);
	}

	if (fileID >= 0 && fileID < (int)m_FileTable.size())
		return m_FileTable[fileID];
	else
		return L"";
}


//////////////////////////////////////////////////////////////////////////
Value* Script::GetVariableRef(const WideString& name)
{
	Value* ret = NULL;

	if (m_ScopeStack.size() != 0)
	{
		// try current scope
		if (m_ScopeStack.size() > 1)
		{
			ret = m_ScopeStack.back().GetVariableRef(name);
		}

		// try script global scope
		if (ret == NULL)
		{
			if (IsChildThread())
				ret = m_ParentScript->GetScriptGlobalVariableRef(name);
			else
				ret = this->GetScriptGlobalVariableRef(name);
		}
	}

	// not found, try game global variables
	if (ret == NULL) ret = m_OwnerMgr->GetVariableRef(name);

	
	return ret;
}

//////////////////////////////////////////////////////////////////////////
Value* Script::GetScriptGlobalVariableRef(const WideString& name)
{
	return m_ScopeStack.front().GetVariableRef(name);
}

//////////////////////////////////////////////////////////////////////////
void Script::AddLocalVariable(const WideString& name)
{
	m_ScopeStack.back().AddVariable(name);
}

//////////////////////////////////////////////////////////////////////////
void Script::AddGlobalVariable(const WideString& name)
{
	m_OwnerMgr->AddVariable(name);
}


//////////////////////////////////////////////////////////////////////////
int Script::GetEntryPointAddress(const WideString& name) const
{
	if (IsChildThread())
	{
		return m_ParentScript->GetEntryPointAddress(name);
	}

	EntryPointMap::const_iterator it;
	it = m_FunctionTable.find(name);
	if (it != m_FunctionTable.end()) return (*it).second;

	it = m_MethodTable.find(name);
	if (it != m_MethodTable.end()) return (*it).second;

	return -1;
}

//////////////////////////////////////////////////////////////////////////
bool Script::HasMethod(const WideString& name) const
{
	if (IsChildThread())
	{
		return m_ParentScript->HasMethod(name);
	}

	EntryPointMap::const_iterator it;
	it = m_MethodTable.find(name);
	
	return (it != m_MethodTable.end());
}

//////////////////////////////////////////////////////////////////////////
bool Script::WaitForObject(ScriptableObject* object)
{
	if (object && object->GetHandle() >= 0)
	{
		m_WaitForObjectHandle = object->GetHandle();
		SetState(SCRIPT_WAITING_FOR_OBJECT);
		return true;
	}
	else return false;
}

//////////////////////////////////////////////////////////////////////////
bool Script::WaitForScript(ScriptPtr script)
{
	m_WaitForScript = script;
	SetState(SCRIPT_WAITING_FOR_SCRIPT);
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool Script::SleepFor(unsigned long time)
{
	m_SleepEndTime = GetCurrentTime() + time;
	SetState(SCRIPT_SLEEPING);
	return true;
}

//////////////////////////////////////////////////////////////////////////
unsigned long Script::GetCurrentTime()
{
	if (m_OwnerObject) return m_OwnerObject->GetTier()->GetCurrentTime();
	else return 0;
}

//////////////////////////////////////////////////////////////////////////
void Script::ResolveWaitingStates()
{
	switch (m_State)
	{
	case SCRIPT_SLEEPING:
		{
			unsigned long currentTime = GetCurrentTime();
			if (currentTime >= m_SleepEndTime) SetState(SCRIPT_RUNNING);
		}
		break;
	
	case SCRIPT_WAITING_FOR_OBJECT:
		{
			ScriptableObject* obj = Game::GetInstance()->GetObjectByHandle(m_WaitForObjectHandle);
			if (!obj || obj->IsReady()) SetState(SCRIPT_RUNNING);
		}
		break;
	
	case SCRIPT_WAITING_FOR_SCRIPT:
		{
			if (m_WaitForScript->GetState() == SCRIPT_FINISHED_THREAD)
			{
				// grab the return value of the other script and continue
				m_Stack.Push(m_WaitForScript->GetStack()->Pop());
				SetState(SCRIPT_RUNNING);
				
				m_WaitForScript->SetState(SCRIPT_FINISHED);
				m_WaitForScript.reset();
			}
			else if (m_WaitForScript->GetState() == SCRIPT_FINISHED || m_WaitForScript->GetState() == SCRIPT_ERROR)
			{
				// something went wrong with the other script; just push null and move on
				m_Stack.PushNull();
				SetState(SCRIPT_RUNNING);

				m_WaitForScript.reset();
			}
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
void Script::AddRootsToGrayList(ValueManager* valMgr)
{
	m_Stack.AddRootsToList(valMgr);

	foreach (ScriptScope& scope, m_ScopeStack)
	{
		scope.AddRootsToGrayList(valMgr);
	}
}


} // namespace Wme
