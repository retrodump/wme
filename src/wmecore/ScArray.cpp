// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "ScArray.h"
#include <boost/lexical_cast.hpp>
#include "StringUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
ScArray::ScArray()
{
	m_IsRefCounted = true;
	m_Length = 0;
}

//////////////////////////////////////////////////////////////////////////
ScArray::~ScArray()
{
}

//////////////////////////////////////////////////////////////////////////
bool ScArray::Initialize(Script* script)
{
	int numParams = script->GetStack()->Pop()->GetInt();

	if (numParams == 0)
	{
		m_Length = 0;
	}
	else if (numParams == 1)
	{
		m_Length = script->GetStack()->Pop()->GetInt();
		if (m_Length < 0) m_Length = 0;
	}
	else
	{
		m_Length = 0;
		for (int i = 0; i < numParams; i++)
		{
			Value* val = script->GetStack()->Pop();
			Push(val);
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ScArray::OnSetGenericProperty(const WideString& name, Value* val)
{
	int index = NormalizeIndex(name);
	if (index < 0) return true;

	// expand length if necessary
	if (index >= m_Length) m_Length = index + 1;

	return true;
}

//////////////////////////////////////////////////////////////////////////
int ScArray::NormalizeIndex(const WideString& index)
{
	try
	{
		return boost::lexical_cast<int>(index);
	}
	catch (boost::bad_lexical_cast&)
	{
		return -1;
	}
}

//////////////////////////////////////////////////////////////////////////
void ScArray::Push(Value* val)
{
	WideString propName = StringUtil::ToString(m_Length);
	SetProperty(propName, val);
}

//////////////////////////////////////////////////////////////////////////
WideString ScArray::ConvertToString()
{
	WideString str;

	for (int i = 0; i < m_Length; i++)
	{
		WideString propName = StringUtil::ToString(i);
		Value* val = GetProperty(propName);

		if (i > 0) str += L",";
		str += val->GetString();
	}
	return str;
}

//////////////////////////////////////////////////////////////////////////
// script interface
//////////////////////////////////////////////////////////////////////////
void ScArray::RegisterScriptInterface()
{
	ScriptableObject::RegisterScriptInterface();

	RegisterScriptMethod(L"Push",		(MethodHandler)&ScArray::ScPush);
	RegisterScriptMethod(L"Pop",		(MethodHandler)&ScArray::ScPop);

	RegisterScriptProp(L"Length",		(PropertyHandler)&ScArray::ScPropLength, false);
	RegisterScriptProp(L"Empty",		(PropertyHandler)&ScArray::ScPropEmpty, true);
}

//////////////////////////////////////////////////////////////////////////
bool ScArray::ScPush(Script* script, const WideString& methodName, bool async)
{
	int numParams = script->GetStack()->Pop()->GetInt();

	for (int i = 0; i < numParams; i++)
	{
		Value* val = script->GetStack()->Pop();
		Push(val);
	}
	script->GetStack()->Push(m_Length);

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ScArray::ScPop(Script* script, const WideString& methodName, bool async)
{
	script->GetStack()->CheckParams(0);
	if (m_Length == 0) script->GetStack()->PushNull();
	else
	{
		m_Length--;
		WideString propName = StringUtil::ToString(m_Length);
		Value* val = GetProperty(propName);
		DeleteProperty(propName);

		script->GetStack()->Push(val);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ScArray::ScPropLength(const WideString& prop, Value* val, bool isSetting)
{
	if (isSetting)
	{
		int origLength = m_Length;
		m_Length = val->GetInt();
		if (m_Length < 0) m_Length = 0;

		// do we need to trim values?
		if (m_Length < origLength)
		{
			for (int i = m_Length; i < origLength; i++)
			{
				DeleteProperty(StringUtil::ToString(i));
			}
		}
	}
	else
	{
		val->SetValue(m_Length);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ScArray::ScPropEmpty(const WideString& prop, Value* val, bool isSetting)
{
	val->SetValue(m_Length == 0);
	return true;
}


} // namespace Wme
