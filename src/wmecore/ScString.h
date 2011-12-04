// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeScString_H__
#define __WmeScString_H__


#include "ScriptableObject.h"


namespace Wme
{
	class WmeDllExport ScString : public ScriptableObject
	{
	public:
		
		ScString();
		~ScString();

        // ScriptableObject
		RTTI(ScString);
		virtual WideString GetTypeName() const { return L"String"; }
		WideString ConvertToString();
		
		void RegisterScriptInterface();

		void SetString(const WideString& val) { m_String = val; }
		WideString GetString() const { return m_String; }

	private:
		WideString m_String;

		bool ScToLower(Script* script, const WideString& methodName, bool async);
		bool ScToUpper(Script* script, const WideString& methodName, bool async);
		bool ScSubstring(Script* script, const WideString& methodName, bool async);
		bool ScCompare(Script* script, const WideString& methodName, bool async);
		bool ScStartsWith(Script* script, const WideString& methodName, bool async);
		bool ScEndsWith(Script* script, const WideString& methodName, bool async);
		bool ScSplit(Script* script, const WideString& methodName, bool async);
		bool ScTrim(Script* script, const WideString& methodName, bool async);
		bool ScTrimStart(Script* script, const WideString& methodName, bool async);
		bool ScTrimEnd(Script* script, const WideString& methodName, bool async);
		bool ScReplace(Script* script, const WideString& methodName, bool async);
		bool ScIndexOf(Script* script, const WideString& methodName, bool async);
		bool ScLastIndexOf(Script* script, const WideString& methodName, bool async);
		
		bool ScPropLength(const WideString& prop, Value* val, bool isSetting);
		bool ScPropEmpty(const WideString& prop, Value* val, bool isSetting);
	};
}


#endif // __WmeScString_H__