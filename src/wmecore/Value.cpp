// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Value.h"
#include "ValueManager.h"
#include "ScriptableObject.h"
#include "Game.h"
#include "ScriptManager.h"
#include "ScString.h"
#include <boost/lexical_cast.hpp>



namespace Wme
{


//////////////////////////////////////////////////////////////////////////
Value::Value()
{
	m_ValReference = NULL;
	m_Type = VAL_NULL;
	m_GCMark = MARK_WHITE;
	Clear();
}

//////////////////////////////////////////////////////////////////////////
Value::~Value()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////////
Value* Value::Create()
{
	return Game::GetInstance()->GetValueMgr()->CreateValue();
}

//////////////////////////////////////////////////////////////////////////
Value* Value::Create(int val)
{
	Value* newVal = Game::GetInstance()->GetValueMgr()->CreateValue();
	newVal->SetValue(val);
	return newVal;
}

//////////////////////////////////////////////////////////////////////////
Value* Value::Create(const WideString& val)
{
	Value* newVal = Game::GetInstance()->GetValueMgr()->CreateValue();
	newVal->SetValue(val);
	return newVal;
}

//////////////////////////////////////////////////////////////////////////
Value* Value::Create(double val)
{
	Value* newVal = Game::GetInstance()->GetValueMgr()->CreateValue();
	newVal->SetValue(val);
	return newVal;
}

//////////////////////////////////////////////////////////////////////////
Value* Value::Create(bool val)
{
	Value* newVal = Game::GetInstance()->GetValueMgr()->CreateValue();
	newVal->SetValue(val);
	return newVal;
}

//////////////////////////////////////////////////////////////////////////
Value* Value::Create(ScriptableObject* val)
{
	Value* newVal = Game::GetInstance()->GetValueMgr()->CreateValue();
	newVal->SetValue(val);
	return newVal;
}


//////////////////////////////////////////////////////////////////////////
void Value::Clear()
{
	if (m_Type == VAL_NATIVE)
	{
		ScriptableObject* obj = Game::GetInstance()->GetObjectByHandle(m_ValNative);
		if (obj) obj->RemoveRef();
	}

	m_Type = VAL_NULL;

	m_ValString = L"";
	m_ValInt = 0;
	m_ValDouble = 0.0;
	m_ValBool = false;
	m_Properties.clear();
	m_PropReference = L"";
	m_ValReference = NULL;
	m_ValNative = -1;
}

//////////////////////////////////////////////////////////////////////////
void Value::ClearValue()
{
	if (m_Type == VAL_REFERENCE)
	{
		if (m_PropReference.empty())
		{
			m_ValReference->ClearValue();
			return;
		}
		else
		{
			if (m_ValReference->HasProperty(m_PropReference))
			{
				m_ValReference->GetProperty(m_PropReference)->ClearValue();
			}
			return;
		}
	}
	Clear();
}


//////////////////////////////////////////////////////////////////////////
Value::ValueType Value::GetType() const
{
	switch (m_Type)
	{
	case VAL_REFERENCE:
		if (m_PropReference.empty())
			return m_ValReference->GetType();
		else
		{
			if (m_ValReference->HasProperty(m_PropReference)) return m_ValReference->GetProperty(m_PropReference)->GetType();
			else return VAL_NULL;
		}

	default:
		return m_Type;
	}
}

//////////////////////////////////////////////////////////////////////////
Value::ValueType Value::GetExactType() const
{
	return m_Type;
}

//////////////////////////////////////////////////////////////////////////
void Value::SetNull()
{
	switch (m_Type)
	{
	case VAL_REFERENCE:
		if (m_PropReference.empty())
			return m_ValReference->SetNull();
		else
			return m_ValReference->GetPropertySafe(m_PropReference)->SetNull();

	default:
		Clear();
	}
}

//////////////////////////////////////////////////////////////////////////
void Value::SetValue(int val)
{
	if (m_Type == VAL_REFERENCE && m_ValReference->GetType() == VAL_NATIVE)
	{
		ScriptableObject* obj = m_ValReference->GetNative();
		if(obj && obj->HasNativeProperty(m_PropReference))
		{
			Value* valToSet = Game::GetInstance()->GetValueMgr()->CreateValue();
			valToSet->SetValue(val);
			SetValue(valToSet);
			return;
		}
	}


	switch (m_Type)
	{
	case VAL_REFERENCE:
		if (m_PropReference.empty())
			return m_ValReference->SetValue(val);
		else
			return m_ValReference->GetPropertySafe(m_PropReference)->SetValue(val);

	default:
		Clear();
		m_ValInt = val;
		m_Type = VAL_INT;
	}
}

//////////////////////////////////////////////////////////////////////////
void Value::SetValue(const WideString& val)
{
	if (m_Type == VAL_REFERENCE && m_ValReference->GetType() == VAL_NATIVE)
	{
		ScriptableObject* obj = m_ValReference->GetNative();
		if(obj && obj->HasNativeProperty(m_PropReference))
		{
			Value* valToSet = Game::GetInstance()->GetValueMgr()->CreateValue();
			valToSet->SetValue(val);
			SetValue(valToSet);
			return;
		}
	}


	switch (m_Type)
	{
	case VAL_REFERENCE:
		if (m_PropReference.empty())
			return m_ValReference->SetValue(val);
		else
			return m_ValReference->GetPropertySafe(m_PropReference)->SetValue(val);

	default:
		Clear();
		m_ValString = val;
		m_Type = VAL_STRING;
	}
}

//////////////////////////////////////////////////////////////////////////
void Value::SetValue(double val)
{
	if (m_Type == VAL_REFERENCE && m_ValReference->GetType() == VAL_NATIVE)
	{
		ScriptableObject* obj = m_ValReference->GetNative();
		if(obj && obj->HasNativeProperty(m_PropReference))
		{
			Value* valToSet = Game::GetInstance()->GetValueMgr()->CreateValue();
			valToSet->SetValue(val);
			SetValue(valToSet);
			return;
		}
	}


	switch (m_Type)
	{
	case VAL_REFERENCE:
		if (m_PropReference.empty())
			return m_ValReference->SetValue(val);
		else
			return m_ValReference->GetPropertySafe(m_PropReference)->SetValue(val);

	default:
		Clear();
		m_ValDouble = val;
		m_Type = VAL_DOUBLE;
	}
}

//////////////////////////////////////////////////////////////////////////
void Value::SetValue(bool val)
{
	if (m_Type == VAL_REFERENCE && m_ValReference->GetType() == VAL_NATIVE)
	{
		ScriptableObject* obj = m_ValReference->GetNative();
		if(obj && obj->HasNativeProperty(m_PropReference))
		{
			Value* valToSet = Game::GetInstance()->GetValueMgr()->CreateValue();
			valToSet->SetValue(val);
			SetValue(valToSet);
			return;
		}
	}


	switch (m_Type)
	{
	case VAL_REFERENCE:
		if (m_PropReference.empty())
			return m_ValReference->SetValue(val);
		else
			return m_ValReference->GetPropertySafe(m_PropReference)->SetValue(val);

	default:
		Clear();
		m_ValBool = val;
		m_Type = VAL_BOOL;
	}
}

//////////////////////////////////////////////////////////////////////////
void Value::SetValue(ScriptableObject* val)
{
	switch (m_Type)
	{
	case VAL_REFERENCE:
		if (m_PropReference.empty())
			return m_ValReference->SetValue(val);
		else
			return m_ValReference->GetPropertySafe(m_PropReference)->SetValue(val);

	default:
		Clear();
		if (val)
		{
			m_ValNative = val->GetHandle();
			m_Type = VAL_NATIVE;
			val->AddRef();
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void Value::SetReference(Value* val, const WideString& prop)
{
	Clear();
	m_ValReference = val;
	m_PropReference = prop;
	m_Type = VAL_REFERENCE;

	Game::GetInstance()->GetValueMgr()->WriteBarrier(this, val);
}

//////////////////////////////////////////////////////////////////////////
bool Value::IsNull() const
{
	if (m_Type == VAL_REFERENCE)
	{
		if (m_PropReference.empty())
			return m_ValReference->IsNull();
		else
			return m_ValReference->GetProperty(m_PropReference)->IsNull();
	}
	else return m_Type == VAL_NULL;

}

//////////////////////////////////////////////////////////////////////////
int Value::GetInt(int defaultVal) const
{
	switch (m_Type)
	{
	case VAL_REFERENCE:
		if (m_PropReference.empty())
			return m_ValReference->GetInt(defaultVal);
		else
			return m_ValReference->GetProperty(m_PropReference)->GetInt();
	
	case VAL_NULL:
		return defaultVal;
	
	case VAL_INT:
		return m_ValInt;
	
	case VAL_STRING:
		try
		{
			return boost::lexical_cast<int>(m_ValString);
		}
		catch (boost::bad_lexical_cast&)
		{
			return 0;
		}
	
	case VAL_BOOL:
		return (int)m_ValBool;
	
	case VAL_DOUBLE:
		return (int)m_ValDouble;

	case VAL_NATIVE:
		ScriptableObject* obj = Game::GetInstance()->GetObjectByHandle(m_ValNative);
		if (obj) return obj->ConvertToInt();
		break;

	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
WideString Value::GetString(const WideString& defaultVal) const
{
	switch (m_Type)
	{
	case VAL_REFERENCE:
		if (m_PropReference.empty())
			return m_ValReference->GetString();
		else
			return m_ValReference->GetProperty(m_PropReference)->GetString(defaultVal);

	case VAL_NULL:
		return defaultVal;

	case VAL_INT:
		return boost::lexical_cast<WideString>(m_ValInt);

	case VAL_STRING:
		return m_ValString;

	case VAL_BOOL:
		return m_ValBool?L"true":L"false";

	case VAL_DOUBLE:
		return boost::lexical_cast<WideString>(m_ValDouble);

	case VAL_OBJECT:
		return L"(object)";

	case VAL_NATIVE:
		ScriptableObject* obj = Game::GetInstance()->GetObjectByHandle(m_ValNative);
		if (obj) return obj->ConvertToString();
		break;

	}
	return L"";
}

//////////////////////////////////////////////////////////////////////////
double Value::GetDouble(double defaultVal) const
{
	switch (m_Type)
	{
	case VAL_REFERENCE:
		if (m_PropReference.empty())
			return m_ValReference->GetDouble(defaultVal);
		else
			return m_ValReference->GetProperty(m_PropReference)->GetDouble();

	case VAL_NULL:
		return defaultVal;

	case VAL_INT:
		return (double)m_ValInt;

	case VAL_STRING:
		try
		{
			return boost::lexical_cast<double>(m_ValString);
		}
		catch (boost::bad_lexical_cast&)
		{
			return 0.0;
		}

	case VAL_BOOL:
		return (double)m_ValBool;

	case VAL_DOUBLE:
		return m_ValDouble;

	case VAL_NATIVE:
		ScriptableObject* obj = Game::GetInstance()->GetObjectByHandle(m_ValNative);
		if (obj) return obj->ConvertToDouble();
		break;

	}
	return 0.0;
}

//////////////////////////////////////////////////////////////////////////
bool Value::GetBool(bool defaultVal) const
{
	switch (m_Type)
	{
	case VAL_REFERENCE:
		if (m_PropReference.empty())
			return m_ValReference->GetBool(defaultVal);
		else
			return m_ValReference->GetProperty(m_PropReference)->GetBool();

	case VAL_NULL:
		return defaultVal;

	case VAL_INT:
		return (m_ValInt != 0);

	case VAL_STRING:
		return !m_ValString.empty();

	case VAL_BOOL:
		return m_ValBool;

	case VAL_DOUBLE:
		return (m_ValDouble != 0.0);

	case VAL_OBJECT:
		return true;

	case VAL_NATIVE:
		ScriptableObject* obj = Game::GetInstance()->GetObjectByHandle(m_ValNative);
		if (obj) return obj->ConvertToBool();
		break;

	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
ScriptableObject* Value::GetNative() const
{
	if (m_Type == VAL_REFERENCE)
	{
		if (m_PropReference.empty())
			return m_ValReference->GetNative();
		else
			return m_ValReference->GetProperty(m_PropReference)->GetNative();
	}
	else if (m_Type == VAL_NATIVE) return Game::GetInstance()->GetObjectByHandle(m_ValNative);
	else return NULL;
}

//////////////////////////////////////////////////////////////////////////
Value* Value::GetReference() const
{
	if (m_Type == VAL_REFERENCE) return m_ValReference;
	else return NULL;
}

//////////////////////////////////////////////////////////////////////////
WideString Value::GetPropReference() const
{
	if (m_Type == VAL_REFERENCE) return m_PropReference;
	else return L"";
}

//////////////////////////////////////////////////////////////////////////
void Value::SetProperty(const WideString& name, Value* val)
{
	switch (m_Type)
	{
	case VAL_REFERENCE:
		if (m_PropReference.empty())
			m_ValReference->SetProperty(name, val);
		else
			m_ValReference->GetPropertySafe(m_PropReference)->SetProperty(name, val);
		break;

	case VAL_NATIVE:
		{
			ScriptableObject* obj = Game::GetInstance()->GetObjectByHandle(m_ValNative);
			if (obj) obj->SetProperty(name, val);
			break;
		}

	case VAL_STRING:
		{
			ScString* string = Game::GetInstance()->GetScriptMgr()->GetStringWrapper();
			string->SetString(m_ValString);
			string->SetProperty(name, val);
			m_ValString = string->GetString();
			break;
		}

	default:	
		if (m_Type != VAL_OBJECT)
		{
			Clear();
			m_Type = VAL_OBJECT;
		}
		if (val == NULL || val->IsValueType())
		{
			Value* newVal = Game::GetInstance()->GetValueMgr()->CreateValue();
			if (val) newVal->SetValue(val);
			m_Properties[name] = newVal;
			Game::GetInstance()->GetValueMgr()->WriteBarrier(this, newVal);
		}
		else
		{
			m_Properties[name] = val;
			Game::GetInstance()->GetValueMgr()->WriteBarrier(this, val);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
Value* Value::GetPropertySafe(const WideString& name)
{
	if (!HasProperty(name))
	{
		Value* nullVal = Game::GetInstance()->GetValueMgr()->CreateValue();
		if (m_Type == VAL_NATIVE)
		{
			ScriptableObject* obj = Game::GetInstance()->GetObjectByHandle(m_ValNative);
			if (obj) obj->SetProperty(name, nullVal);
			else SetProperty(name, nullVal);
		}
		else SetProperty(name, nullVal);
	}
	return GetProperty(name);
}


//////////////////////////////////////////////////////////////////////////
Value* Value::GetProperty(const WideString& name) const
{
	switch (m_Type)
	{
	case VAL_REFERENCE:
		if (m_PropReference.empty())
			return m_ValReference->GetProperty(name);
		else
		{
			Value* propRef = m_ValReference->GetProperty(m_PropReference);
			if (propRef) return propRef->GetProperty(name);
			else
			{
				Value* nullVal = Game::GetInstance()->GetValueMgr()->CreateValue();
				return nullVal;
			}
		}

	case VAL_NATIVE:
		{
			Value* retVal = NULL;
			ScriptableObject* obj = Game::GetInstance()->GetObjectByHandle(m_ValNative);
			if (obj) retVal = obj->GetProperty(name);
			
			if (retVal) return retVal;
			else return Value::Create();
		}
		break;

	case VAL_STRING:
		{
			ScString* string = Game::GetInstance()->GetScriptMgr()->GetStringWrapper();
			string->SetString(m_ValString);
			Value* retVal = string->GetProperty(name);
			if (retVal) return retVal;
			else return Value::Create();
		}

	default:
		ValueCollection::const_iterator it = m_Properties.find(name);
		if (it != m_Properties.end()) return (*it).second;
		else
		{
			Value* nullVal = Game::GetInstance()->GetValueMgr()->CreateValue();
			return nullVal;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
bool Value::HasProperty(const WideString& name) const
{
	switch (m_Type)
	{
	case VAL_REFERENCE:
		if (m_PropReference.empty())
			return m_ValReference->HasProperty(name);
		else
			if (m_ValReference->HasProperty(m_PropReference))
				return m_ValReference->GetProperty(m_PropReference)->HasProperty(name);
			else
				return false;

	case VAL_NATIVE:
		{
			ScriptableObject* obj = Game::GetInstance()->GetObjectByHandle(m_ValNative);
			if (obj) return obj->HasProperty(name);
			else return false;
		}
		break;

	case VAL_STRING:
		{
			return Game::GetInstance()->GetScriptMgr()->GetStringWrapper()->HasProperty(name);
		}
		break;

	default:
		ValueCollection::const_iterator it = m_Properties.find(name);
		return (it != m_Properties.end());
	}
}


//////////////////////////////////////////////////////////////////////////
void Value::SetValue(Value* val)
{
	// if we are reference to object, leave the object alone and mutate into value
	if (m_Type == VAL_REFERENCE && GetType() == VAL_OBJECT)
	{
		m_Type = VAL_NULL;
		SetNull();
	}
	
	Value* valToSet = val->GetValue();


	// special case -> we're setting native property, we need to call setter directly
	// (there's nothing to reference, because native props don't physically exist)
	if (m_Type == VAL_REFERENCE && m_ValReference->GetType() == VAL_NATIVE)
	{
		ScriptableObject* obj = m_ValReference->GetNative();
		if (obj && obj->HasNativeProperty(GetPropReference()))
		{
			obj->SetProperty(GetPropReference(), valToSet);
			return;
		}
	}


	ClearValue();

	if (valToSet->IsValueType())
	{
		switch (valToSet->GetType())
		{
		case VAL_NULL:
			SetNull();
			break;

		case VAL_STRING:
			SetValue(valToSet->GetString());
			break;

		case VAL_INT:
			SetValue(valToSet->GetInt());
			break;

		case VAL_DOUBLE:
			SetValue(valToSet->GetDouble());
			break;

		case VAL_BOOL:
			SetValue(valToSet->GetBool());
			break;

		case VAL_NATIVE:
			ScriptableObject* obj = valToSet->GetNative();
			if (obj) SetValue(obj);
			else SetNull();
			break;
		}
	}
	else SetReference(valToSet, L"");
}

//////////////////////////////////////////////////////////////////////////
Value* Value::GetValue()
{
	if (m_Type == VAL_REFERENCE)
	{
		if (m_PropReference.empty())
			return m_ValReference->GetValue();
		else
			return m_ValReference->GetProperty(m_PropReference)->GetValue();

	}
	return this;
}

//////////////////////////////////////////////////////////////////////////
int Value::Compare(const Value* val) const
{
	if (this->GetType() == VAL_NULL && val->GetType() == VAL_NULL) return 0;

	// compare as numbers
	if ((this->GetType() == VAL_INT || this->GetType() == VAL_DOUBLE || this->GetType() == VAL_BOOL) && (val->GetType() == VAL_INT || val->GetType() == VAL_DOUBLE || val->GetType() == VAL_BOOL))
	{
		double val1 = GetDouble();
		double val2 = val->GetDouble();

		if (val1 < val2) return -1;
		else if (val1 > val2) return 1;
		else return 0;
	}

	// compare as native objects
	if (this->GetType() == VAL_NATIVE && val->GetType() == VAL_NATIVE)
	{
		ScriptableObject* val1 = this->GetNative();
		ScriptableObject* val2 = val->GetNative();

		if (val1 == val2) return 0;
		else if (val1 == NULL && val2 != NULL) return -1;
		else if (val1 != NULL && val2 == NULL) return 1;
		else return val1->Compare(val2);
	}

	// compare as objects
	if (this->GetType() == VAL_OBJECT && val->GetType() == VAL_OBJECT)
	{
		ValueCollection props1 = this->GetProperties();
		ValueCollection props2 = val->GetProperties();

		if (props1.size() < props2.size()) return -1;
		else if (props1.size() > props2.size()) return 1;
		else
		{
			ValueCollection::const_iterator it;
			for (it = props1.begin(); it != props1.end(); it++)
			{
				if (!val->HasProperty((*it).first)) return 1;

				Value* val1 = (*it).second;
				Value* val2 = val->GetProperty((*it).first);

				// this is pretty silly but at least it gives us some determinability
				int res = val1->Compare(val2);
				if (res != 0) return res;
			}
		}
		return 0;
	}

	// compare as booleans
	if (this->GetType() == VAL_BOOL || val->GetType() == VAL_BOOL)
	{
		bool val1 = this->GetBool();
		bool val2 = val->GetBool();

		if (val1 == val2) return 0;
		else if (val1 && !val2) return 1;
		else return -1;
	}

	// otherwise just compare as strings
	WideString val1 = this->GetString();
	WideString val2 = val->GetString();

	return val1.compare(val2);
}

//////////////////////////////////////////////////////////////////////////
bool Value::StrongEquals(const Value* val) const
{
	if (this->GetType() != val->GetType()) return false;

	switch (this->GetType())
	{
	case VAL_STRING:
		return (m_ValString == val->GetString());

	case VAL_INT:
		return (m_ValInt == val->GetInt());

	case VAL_DOUBLE:
		return (m_ValDouble == val->GetDouble());

	case VAL_BOOL:
		return (m_ValBool == val->GetBool());

	case VAL_OBJECT:
		{			
			ValueCollection::const_iterator it;
			for (it = m_Properties.begin(); it != m_Properties.end(); it++)
			{
				if (!val->HasProperty((*it).first)) return false;
				
				if (!(*it).second->StrongEquals(val->GetProperty((*it).first))) return false;
			}
			return true;
		}

	case VAL_NATIVE:
		return (val->GetNative() == GetNative());

	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
WideString Value::GetTypeName() const
{
	switch (GetType())
	{
	case VAL_STRING:
		return L"string";

	case VAL_INT:
		return L"integer";

	case VAL_DOUBLE:
		return L"float";

	case VAL_BOOL:
		return L"boolean";

	case VAL_OBJECT:
		return L"object";

	case VAL_NATIVE:
		{
			ScriptableObject* obj = GetNative();
			if (obj) return obj->GetTypeName();
		}
		break;
	}
	return L"unknown";
}


//////////////////////////////////////////////////////////////////////////
bool Value::IsValidForArithmetics(const Value* val)
{
	if (GetType() == VAL_NULL) return false;

	if (GetType() == VAL_NATIVE || val->GetType() == VAL_NATIVE)
	{
		// TODO throw exception
		return false;
	}

	if (GetType() == VAL_OBJECT || val->GetType() == VAL_OBJECT)
	{
		// TODO throw exception
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
void Value::Add(const Value* val)
{
	if (!IsValidForArithmetics(val)) return;

	if (val->GetType() == VAL_NULL)
	{
		SetNull();
		return;
	}

	if (GetType() == VAL_STRING || val->GetType() == VAL_STRING)
	{
		SetValue(GetString() + val->GetString());
		return;
	}

	if (GetType() == VAL_DOUBLE || val->GetType() == VAL_DOUBLE)
	{
		SetValue(GetDouble() + val->GetDouble());
		return;
	}

	SetValue(GetInt() + val->GetInt());
}

//////////////////////////////////////////////////////////////////////////
void Value::Sub(const Value* val)
{
	if (!IsValidForArithmetics(val)) return;

	if (val->GetType() == VAL_NULL)
	{
		SetNull();
		return;
	}

	if (GetType() == VAL_DOUBLE || val->GetType() == VAL_DOUBLE)
	{
		SetValue(GetDouble() - val->GetDouble());
		return;
	}

	SetValue(GetInt() - val->GetInt());
}

//////////////////////////////////////////////////////////////////////////
void Value::Mul(const Value* val)
{
	if (!IsValidForArithmetics(val)) return;

	if (val->GetType() == VAL_NULL)
	{
		SetNull();
		return;
	}

	if (GetType() == VAL_DOUBLE || val->GetType() == VAL_DOUBLE)
	{
		SetValue(GetDouble() * val->GetDouble());
		return;
	}

	SetValue(GetInt() * val->GetInt());
}

//////////////////////////////////////////////////////////////////////////
void Value::Div(const Value* val)
{
	if (!IsValidForArithmetics(val)) return;

	if (val->GetType() == VAL_NULL)
	{
		SetNull();
		return;
	}

	if (GetType() == VAL_DOUBLE || val->GetType() == VAL_DOUBLE)
	{
		if (val->GetDouble() == 0.0) SetNull();
		else SetValue(GetDouble() / val->GetDouble());
		return;
	}

	if (val->GetInt() == 0) SetNull();
	else SetValue(GetInt() / val->GetInt());
}

//////////////////////////////////////////////////////////////////////////
void Value::Modulo(const Value* val)
{
	if (!IsValidForArithmetics(val)) return;

	if (val->GetType() == VAL_NULL)
	{
		SetNull();
		return;
	}

	if (GetType() == VAL_DOUBLE || val->GetType() == VAL_DOUBLE)
	{
		if (val->GetDouble() == 0.0) SetNull();
		else SetValue(fmod(GetDouble(), val->GetDouble()));
		return;
	}

	if (val->GetInt() == 0) SetNull();
	else SetValue(GetInt() % val->GetInt());
}

//////////////////////////////////////////////////////////////////////////
void Value::Neg()
{
	SetValue(!GetBool());
}

//////////////////////////////////////////////////////////////////////////
void Value::Increment()
{
	if (GetType() == VAL_DOUBLE)
	{
		SetValue(GetDouble() + 1.0);
		return;
	}
	SetValue(GetInt() + 1);
}

//////////////////////////////////////////////////////////////////////////
void Value::Decrement()
{
	if (GetType() == VAL_DOUBLE)
	{
		SetValue(GetDouble() - 1.0);
		return;
	}
	SetValue(GetInt() - 1);
}


//////////////////////////////////////////////////////////////////////////
void Value::AddPropertiesToGrayList(ValueManager* valMgr) const
{
	foreach (ValueCollection::value_type val, m_Properties)
	{
		valMgr->AddToGrayList(val.second);
	}
}

//////////////////////////////////////////////////////////////////////////
void Value::AddReferencesToGrayList(ValueManager* valMgr) const
{
	AddPropertiesToGrayList(valMgr);
	if (m_ValReference) valMgr->AddToGrayList(m_ValReference);
}

#ifdef WME_GC_WRITE_BARRIER_CHECKING
//////////////////////////////////////////////////////////////////////////
bool Value::HasWhiteReferences() const
{
	if (m_ValReference && m_ValReference->GetGCMark() == Value::MARK_WHITE) return true;

	foreach (ValueCollection::value_type val, m_Properties)
	{
		if (val.second->GetGCMark() == Value::MARK_WHITE) return true;
	}
	return false;
}
#endif


} // namespace Wme
