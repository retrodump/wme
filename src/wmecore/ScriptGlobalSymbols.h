// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeScriptGlobalObjects_H__
#define __WmeScriptGlobalObjects_H__


#include "ScriptableObject.h"

namespace Wme
{
	class WmeDllExport ScriptGlobalSymbols :	public ScriptableObject
	{
	public:
		RTTI(ScriptGlobalSymbols);
		ScriptGlobalSymbols();
		virtual ~ScriptGlobalSymbols();

		typedef bool (ScriptGlobalSymbols::*Constructor)(Script* script, const WideString& className);
		typedef std::map<WideString, Constructor> ConstructorCollection;
		ConstructorCollection m_Constructors;
		void RegisterConstructor(WideString className, Constructor constructor);

		WideStringListPtr GetClassList() const;
		bool InvokeConstructor(Script* script, const WideString& className);


		virtual void RegisterScriptInterface();

	private:
		bool ScSleep(Script* script, const WideString& methodName, bool async);
		bool ScToString(Script* script, const WideString& methodName, bool async);
		bool ScToInt(Script* script, const WideString& methodName, bool async);
		bool ScToFloat(Script* script, const WideString& methodName, bool async);
		bool ScToBool(Script* script, const WideString& methodName, bool async);
		
		bool ScPropGame(const WideString& prop, Value* val, bool isSetting);

		bool ScClassArray(Script* script, const WideString& className);
	};
}

#endif // __WmeScriptGlobalObjects_H__