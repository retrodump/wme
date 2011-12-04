// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeScriptManager_H__
#define __WmeScriptManager_H__

#include "Object.h"
#include "Script.h"
#include "Value.h"

namespace Wme
{
	class DynamicBuffer;
	class ScriptGlobalSymbols;
	class ScString;

	class WmeDllExport ScriptManager : public Object
	{
	public:
		ScriptManager();
		virtual ~ScriptManager();
		ScriptPtr RunScript(const WideString& fileName, ScriptableObject* owner);
		void ProcessScripts();
		void AddScript(ScriptPtr script);

		DynamicBuffer* GetCompiledScript(const WideString& fileName);

		Value* GetVariableRef(const WideString& name);
		void AddVariable(const WideString& name);

		void AddRootsToGrayList(ValueManager* valMgr);

		bool InvokeGlobalFunction(Script* script, const WideString& name);
		bool InvokeConstructor(Script* script, const WideString& className);

		ScString* GetStringWrapper() { return m_StringWrapper; }

	private:		
		Scripts m_Scripts;
		Value m_GlobalVars;

		ScString* m_StringWrapper;

		ScriptGlobalSymbols* m_GlobalSymbols;
	};
}

#endif // __WmeScriptManager_H__
