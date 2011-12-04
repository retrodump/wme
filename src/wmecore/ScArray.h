// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeScArray_H__
#define __WmeScArray_H__

#include "ScriptableObject.h"

namespace Wme
{
	class Script;

	class WmeDllExport ScArray : public ScriptableObject
	{
	public:		
		ScArray();
		virtual ~ScArray();

		void Push(Value* val);

		bool Initialize(Script* script);
		virtual bool OnSetGenericProperty(const WideString& name, Value* val);

        // ScriptableObject
		virtual WideString GetTypeName() const { return L"Array"; }	
		virtual WideString ConvertToString();
		RTTI(ScArray);

        // script interface
		virtual void RegisterScriptInterface();

	private:
		int m_Length;
		int NormalizeIndex(const WideString& index);
		
		bool ScPush(Script* script, const WideString& methodName, bool async);
		bool ScPop(Script* script, const WideString& methodName, bool async);
		
		bool ScPropLength(const WideString& prop, Value* val, bool isSetting);
		bool ScPropEmpty(const WideString& prop, Value* val, bool isSetting);
	};
}


#endif // __WmeScArray_H__