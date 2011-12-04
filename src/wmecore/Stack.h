// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeStack_H__
#define __WmeStack_H__

#include <deque>
#include "Value.h"

namespace Wme
{
	class Value;
	class ScriptableObject;

	class WmeDllExport Stack
	{
	public:
		Stack();
		virtual ~Stack();

		void PushNull();
		void Push(Value* val);
		void Push(int val);
		void Push(double val);
		void Push(bool val);
		void Push(const WideString& val);
		void Push(const wchar_t* val);
		void Push(ScriptableObject* val);

		Value* Pop();
		Value* Top();

		void CheckParams(int requiredNumParams);
		void KillParams();
		void CopyParamsFrom(Stack* sourceStack);
		Value* GrabLastParam();

		void AddRootsToList(ValueManager* valMgr) const;

	private:
		typedef std::deque<Value*> ValueStack;
		ValueStack m_Values;
	};
}

#endif // __WmeStack_H__
