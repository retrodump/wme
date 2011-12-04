// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Stack.h"
#include "Game.h"
#include "ValueManager.h"
#include "ScriptableObject.h"
#include <list>

namespace Wme
{


//////////////////////////////////////////////////////////////////////////
Stack::Stack()
{
}

//////////////////////////////////////////////////////////////////////////
Stack::~Stack()
{
}

//////////////////////////////////////////////////////////////////////////
void Stack::PushNull()
{
	Value* valNull = Game::GetInstance()->GetValueMgr()->CreateValue();
	Push(valNull);
}

//////////////////////////////////////////////////////////////////////////
void Stack::Push(Value* val)
{
	m_Values.push_back(val);
}

//////////////////////////////////////////////////////////////////////////
void Stack::Push(int val)
{
	Value* intVal = Game::GetInstance()->GetValueMgr()->CreateValue();
	intVal->SetValue(val);
	Push(intVal);
}

//////////////////////////////////////////////////////////////////////////
void Stack::Push(double val)
{
	Value* doubleVal = Game::GetInstance()->GetValueMgr()->CreateValue();
	doubleVal->SetValue(val);
	Push(doubleVal);
}

//////////////////////////////////////////////////////////////////////////
void Stack::Push(bool val)
{
	Value* boolVal = Game::GetInstance()->GetValueMgr()->CreateValue();
	boolVal->SetValue(val);
	Push(boolVal);
}

//////////////////////////////////////////////////////////////////////////
void Stack::Push(const WideString& val)
{
	Value* stringVal = Game::GetInstance()->GetValueMgr()->CreateValue();
	stringVal->SetValue(val);
	Push(stringVal);
}

//////////////////////////////////////////////////////////////////////////
void Stack::Push(const wchar_t* val)
{
	Value* stringVal = Game::GetInstance()->GetValueMgr()->CreateValue();
	stringVal->SetValue(WideString(val));
	Push(stringVal);
}

//////////////////////////////////////////////////////////////////////////
void Stack::Push(ScriptableObject* val)
{
	Value* nativeVal = Game::GetInstance()->GetValueMgr()->CreateValue();
	nativeVal->SetValue(val);
	Push(nativeVal);
}

//////////////////////////////////////////////////////////////////////////
Value* Stack::Pop()
{
	if (m_Values.empty())
	{
		Game::GetInstance()->Log(L"Stack underrun.");
		// TODO: throw script error exception
		Value* nullVal = Game::GetInstance()->GetValueMgr()->CreateValue();
		return nullVal;
	}

	Value* ret = m_Values.back();
	m_Values.pop_back();

	return ret;
}

//////////////////////////////////////////////////////////////////////////
Value* Stack::Top()
{
	if (m_Values.empty())
	{
		Game::GetInstance()->Log(L"Stack underrun.");
		// TODO: throw script error exception
		Value* nullVal = Game::GetInstance()->GetValueMgr()->CreateValue();
		return nullVal;
	}

	return m_Values.back();
}


//////////////////////////////////////////////////////////////////////////
void Stack::CheckParams(int requiredNumParams)
{
	int numParamsOnStack = Pop()->GetInt();


	typedef std::list<Value*> ValueList;
	ValueList values;

	// pop params from stack
	for (int i = numParamsOnStack; i > 0; i--)
	{
		Value* val = Pop();
		if (i <= requiredNumParams) values.push_front(val);
	}

	// we have too few params, push NULLs for the missing ones
	if (numParamsOnStack < requiredNumParams)
	{
		for (int i = 0; i < (requiredNumParams - numParamsOnStack); i++)
		{
			values.push_back(Game::GetInstance()->GetValueMgr()->CreateValue());
		}
	}

	// copy params to stack in the correct order
	ValueList::reverse_iterator it;
	for (it = values.rbegin(); it != values.rend(); it++)
	{
		Push((*it));
	}
}


//////////////////////////////////////////////////////////////////////////
void Stack::KillParams()
{
	int numParamsOnStack = Pop()->GetInt();
	for (int i = 0; i < numParamsOnStack; i++)
	{
		Pop();
	}
}

//////////////////////////////////////////////////////////////////////////
Value* Stack::GrabLastParam()
{
	int numParamsOnStack = Pop()->GetInt();
	if (!numParamsOnStack)
	{
		Push(0);
		return Value::Create();
	}

	typedef std::list<Value*> ValueList;
	ValueList values;

	// pop params from stack
	for (int i = numParamsOnStack; i > 1; i--)
	{
		Value* val = Pop();
		values.push_front(val);
	}
	Value* ret = Pop();

	foreach (Value* val, values)
	{
		Push(val);
	}
	Push(numParamsOnStack - 1);

	return ret;
}

//////////////////////////////////////////////////////////////////////////
void Stack::CopyParamsFrom(Stack* sourceStack)
{
	Stack tempStack;

	int numParamsOnStack = sourceStack->Pop()->GetInt();
	
	// copy to temp stack
	for (int i = 0; i < numParamsOnStack; i++)
	{
		tempStack.Push(sourceStack->Pop());
	}
	
	// copy to this stack
	for (int i = 0; i < numParamsOnStack; i++)
	{
		Push(tempStack.Pop());
	}

	Push(numParamsOnStack);
}


//////////////////////////////////////////////////////////////////////////
void Stack::AddRootsToList(ValueManager* valMgr) const
{
	ValueStack::const_iterator it;
	for (it = m_Values.begin(); it != m_Values.end(); it++)
	{
		valMgr->AddToGrayList(*it);
	}
}


} // namespace Wme
