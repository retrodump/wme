// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "ScString.h"
#include "StringUtil.h"
#include "ScArray.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
ScString::ScString()
{
	m_CanAttachScripts = false;
	m_CanSetGenericProps = false;
}

//////////////////////////////////////////////////////////////////////////
ScString::~ScString()
{
}

//////////////////////////////////////////////////////////////////////////
WideString ScString::ConvertToString()
{
	return m_String;
}

//////////////////////////////////////////////////////////////////////////
// script interface
//////////////////////////////////////////////////////////////////////////
void ScString::RegisterScriptInterface()
{
	ScriptableObject::RegisterScriptInterface();

	RegisterScriptMethod(L"ToLower",		(MethodHandler)&ScString::ScToLower);
	RegisterScriptMethod(L"ToUpper",		(MethodHandler)&ScString::ScToUpper);
	RegisterScriptMethod(L"Substring",		(MethodHandler)&ScString::ScSubstring);
	RegisterScriptMethod(L"Compare",		(MethodHandler)&ScString::ScCompare);
	RegisterScriptMethod(L"StartsWith",		(MethodHandler)&ScString::ScStartsWith);
	RegisterScriptMethod(L"EndsWith",		(MethodHandler)&ScString::ScEndsWith);
	RegisterScriptMethod(L"Split",			(MethodHandler)&ScString::ScSplit);
	RegisterScriptMethod(L"Trim",			(MethodHandler)&ScString::ScTrim);
	RegisterScriptMethod(L"TrimStart",		(MethodHandler)&ScString::ScTrimStart);
	RegisterScriptMethod(L"TrimEnd",		(MethodHandler)&ScString::ScTrimEnd);
	RegisterScriptMethod(L"Replace",		(MethodHandler)&ScString::ScReplace);
	RegisterScriptMethod(L"IndexOf",		(MethodHandler)&ScString::ScIndexOf);
	RegisterScriptMethod(L"LastIndexOf",	(MethodHandler)&ScString::ScLastIndexOf);

	RegisterScriptProp(L"Length",			(PropertyHandler)&ScString::ScPropLength, true);
	RegisterScriptProp(L"Empty",			(PropertyHandler)&ScString::ScPropEmpty, true);
}

//////////////////////////////////////////////////////////////////////////
bool ScString::ScToLower(Script* script, const WideString& methodName, bool async)
{
	script->GetStack()->CheckParams(0);

	WideString lowerStr = m_String;
	StringUtil::ToLowerCase(lowerStr);

	script->GetStack()->Push(lowerStr);
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ScString::ScToUpper(Script* script, const WideString& methodName, bool async)
{
	script->GetStack()->CheckParams(0);

	WideString upperStr = m_String;
	StringUtil::ToUpperCase(upperStr);

	script->GetStack()->Push(upperStr);
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ScString::ScSubstring(Script* script, const WideString& methodName, bool async)
{
	script->GetStack()->CheckParams(2);
	int from = script->GetStack()->Pop()->GetInt();
	int length = script->GetStack()->Pop()->GetInt(-1);

	from = std::max(from, 0);

	WideString subStr;
	if (length >= 0) subStr = m_String.substr(from, length);
	else subStr = m_String.substr(from);

	script->GetStack()->Push(subStr);
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ScString::ScCompare(Script* script, const WideString& methodName, bool async)
{
	script->GetStack()->CheckParams(2);
	Value* val = script->GetStack()->Pop();
	bool caseInsensitive = script->GetStack()->Pop()->GetBool();
	
	if (val->IsNull())
	{
		script->GetStack()->Push(false);
		return true;
	}

	if (caseInsensitive)
		script->GetStack()->Push(StringUtil::CompareNoCase(m_String, val->GetString()));		
	else
		script->GetStack()->Push(m_String == val->GetString());

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ScString::ScStartsWith(Script* script, const WideString& methodName, bool async)
{
	script->GetStack()->CheckParams(2);
	WideString pattern = script->GetStack()->Pop()->GetString();
	bool caseInsensitive = script->GetStack()->Pop()->GetBool();

	script->GetStack()->Push(StringUtil::StartsWith(m_String, pattern, caseInsensitive));

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ScString::ScEndsWith(Script* script, const WideString& methodName, bool async)
{
	script->GetStack()->CheckParams(2);
	WideString pattern = script->GetStack()->Pop()->GetString();
	bool caseInsensitive = script->GetStack()->Pop()->GetBool();

	script->GetStack()->Push(StringUtil::EndsWith(m_String, pattern, caseInsensitive));

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ScString::ScSplit(Script* script, const WideString& methodName, bool async)
{
	script->GetStack()->CheckParams(2);
	WideString delims = script->GetStack()->Pop()->GetString(L",");
	bool keepEmpty = script->GetStack()->Pop()->GetBool(false);

	WideStringListPtr list = StringUtil::Split(m_String, delims, keepEmpty);

	ScArray* array = new ScArray();
	array->Register();

	WideStringList::const_iterator it;
	for (it = list->begin(); it != list->end(); it++)
	{
		array->Push(Value::Create(*it));
	}
	script->GetStack()->Push(array);

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ScString::ScTrim(Script* script, const WideString& methodName, bool async)
{
	script->GetStack()->CheckParams(1);
	WideString chars = script->GetStack()->Pop()->GetString(L" \r\n\t");

	WideString trimmedStr = StringUtil::Trim(m_String, true, true, chars);
	script->GetStack()->Push(trimmedStr);

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ScString::ScTrimStart(Script* script, const WideString& methodName, bool async)
{
	script->GetStack()->CheckParams(1);
	WideString chars = script->GetStack()->Pop()->GetString(L" \r\n\t");

	WideString trimmedStr = StringUtil::Trim(m_String, true, false, chars);
	script->GetStack()->Push(trimmedStr);

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ScString::ScTrimEnd(Script* script, const WideString& methodName, bool async)
{
	script->GetStack()->CheckParams(1);
	WideString chars = script->GetStack()->Pop()->GetString(L" \r\n\t");

	WideString trimmedStr = StringUtil::Trim(m_String, false, true, chars);
	script->GetStack()->Push(trimmedStr);

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ScString::ScReplace(Script* script, const WideString& methodName, bool async)
{
	script->GetStack()->CheckParams(2);
	Value* valFrom = script->GetStack()->Pop();
	Value* valTo = script->GetStack()->Pop();

	if (valFrom->IsNull() || valTo->IsNull())
	{
		script->GetStack()->Push(m_String);
		return true;
	}

	WideString newStr = StringUtil::Replace(m_String, valFrom->GetString(), valTo->GetString());
	script->GetStack()->Push(newStr);

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ScString::ScIndexOf(Script* script, const WideString& methodName, bool async)
{
	script->GetStack()->CheckParams(2);
	WideString toFind = script->GetStack()->Pop()->GetString();
	int startFrom = script->GetStack()->Pop()->GetInt();

	startFrom = std::max(startFrom, 0);
	startFrom = std::min(startFrom, (int)m_String.length());

	script->GetStack()->Push(StringUtil::IndexOf(m_String, toFind, startFrom));

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ScString::ScLastIndexOf(Script* script, const WideString& methodName, bool async)
{
	script->GetStack()->CheckParams(2);
	WideString toFind = script->GetStack()->Pop()->GetString();
	int startFrom = script->GetStack()->Pop()->GetInt(m_String.length());

	startFrom = std::max(startFrom, 0);
	startFrom = std::min(startFrom, (int)m_String.length());

	script->GetStack()->Push(StringUtil::LastIndexOf(m_String, toFind, startFrom));

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ScString::ScPropLength(const WideString& prop, Value* val, bool isSetting)
{
	val->SetValue((int)m_String.length());
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ScString::ScPropEmpty(const WideString& prop, Value* val, bool isSetting)
{
	val->SetValue(m_String.empty());
	return true;
}


} // namepace Wme
