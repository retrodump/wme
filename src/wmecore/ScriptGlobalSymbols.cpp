// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "ScriptGlobalSymbols.h"
#include "GameScriptable.h"
#include "ScArray.h"

namespace Wme
{


//////////////////////////////////////////////////////////////////////////
ScriptGlobalSymbols::ScriptGlobalSymbols()
{
}

//////////////////////////////////////////////////////////////////////////
ScriptGlobalSymbols::~ScriptGlobalSymbols()
{
}

//////////////////////////////////////////////////////////////////////////
void ScriptGlobalSymbols::RegisterConstructor(WideString className, Constructor constructor)
{
	m_Constructors[className] = constructor;
}

//////////////////////////////////////////////////////////////////////////
WideStringListPtr ScriptGlobalSymbols::GetClassList() const
{
	WideStringListPtr list = WideStringListPtr(new WideStringList());

	foreach (ConstructorCollection::value_type val, m_Constructors)
	{
		list->push_back(val.first);
	}
	return list;
}

//////////////////////////////////////////////////////////////////////////
bool ScriptGlobalSymbols::InvokeConstructor(Script* script, const WideString& className)
{
	ConstructorCollection::iterator it;
	it = m_Constructors.find(className);

	if (it == m_Constructors.end()) return false;
	else
	{
		Constructor c = (*it).second;
		return (this->*c)(script, className);
	}
}

//////////////////////////////////////////////////////////////////////////
// script interface
//////////////////////////////////////////////////////////////////////////
void ScriptGlobalSymbols::RegisterScriptInterface()
{
	// global functions
	RegisterScriptMethod(L"Sleep",		(MethodHandler)&ScriptGlobalSymbols::ScSleep);
	RegisterScriptMethod(L"ToString",	(MethodHandler)&ScriptGlobalSymbols::ScToString);
	RegisterScriptMethod(L"ToInt",		(MethodHandler)&ScriptGlobalSymbols::ScToInt);
	RegisterScriptMethod(L"ToFloat",	(MethodHandler)&ScriptGlobalSymbols::ScToFloat);
	RegisterScriptMethod(L"ToBool",		(MethodHandler)&ScriptGlobalSymbols::ScToBool);
	
	// global variables
	RegisterScriptProp(L"Game",		(PropertyHandler)&ScriptGlobalSymbols::ScPropGame, true);

	// classes
	RegisterConstructor(L"Array", (Constructor)&ScriptGlobalSymbols::ScClassArray);
}

//////////////////////////////////////////////////////////////////////////
bool ScriptGlobalSymbols::ScSleep(Script* script, const WideString& methodName, bool async)
{
	script->GetStack()->CheckParams(1);
	int time = script->GetStack()->Pop()->GetInt();
	script->GetStack()->PushNull();

	script->SleepFor(time);

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ScriptGlobalSymbols::ScToString(Script* script, const WideString& methodName, bool async)
{
	script->GetStack()->CheckParams(1);
	Value* val = script->GetStack()->Pop();
	script->GetStack()->Push(val->GetString());
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ScriptGlobalSymbols::ScToInt(Script* script, const WideString& methodName, bool async)
{
	script->GetStack()->CheckParams(1);
	Value* val = script->GetStack()->Pop();
	script->GetStack()->Push(val->GetInt());
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ScriptGlobalSymbols::ScToFloat(Script* script, const WideString& methodName, bool async)
{
	script->GetStack()->CheckParams(1);
	Value* val = script->GetStack()->Pop();
	script->GetStack()->Push(val->GetDouble());
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ScriptGlobalSymbols::ScToBool(Script* script, const WideString& methodName, bool async)
{
	script->GetStack()->CheckParams(1);
	Value* val = script->GetStack()->Pop();
	script->GetStack()->Push(val->GetBool());
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ScriptGlobalSymbols::ScPropGame(const WideString& prop, Value* val, bool isSetting)
{
	val->SetValue(Game::GetInstance()->GetGameScriptable());
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ScriptGlobalSymbols::ScClassArray(Script* script, const WideString& className)
{
	ScArray* obj = new ScArray();
	obj->Register();

	if (!obj->Initialize(script))
	{
		delete obj;
		return false;
	}

	script->GetStack()->Push((ScriptableObject*)obj);
	return true;
}


} // namespace Wme
