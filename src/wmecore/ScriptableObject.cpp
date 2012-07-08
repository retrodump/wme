// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "ScriptableObject.h"
#include "Game.h"
#include "ScriptManager.h"
#include "Value.h"
#include "ValueManager.h"
#include "XmlUtil.h"
#include "PathUtil.h"


namespace Wme
{


ScriptableObject::ClassMethods ScriptableObject::s_Methods;
ScriptableObject::ClassPropAccessors ScriptableObject::s_PropAccessors;
ScriptableObject::RegisteredClasses ScriptableObject::s_RegisteredClasses;

//////////////////////////////////////////////////////////////////////////
ScriptableObject::ScriptableObject()
{
	m_Handle = -1;
	m_RefCount = 0;
	m_IsRefCounted = false;
	m_Name = L"<unnamed>";
	m_Tier = NULL;

	m_CanSetGenericProps = true;
	m_CanAttachScripts = true;
}

//////////////////////////////////////////////////////////////////////////
ScriptableObject::~ScriptableObject()
{
	if (m_Handle >= 0) Unregister();
	ShutdownScripts();
}

//////////////////////////////////////////////////////////////////////////
void ScriptableObject::ShutdownScripts()
{
	foreach (ScriptPtr script, m_Scripts)
	{
		script->DecoupleOwnerAndFinish();
	}
	m_Scripts.clear();
}

//////////////////////////////////////////////////////////////////////////
int ScriptableObject::Register()
{
	InitializeLookupTables();
	m_Handle = Game::GetInstance()->RegisterObject(this);
	return m_Handle;
}

//////////////////////////////////////////////////////////////////////////
void ScriptableObject::Unregister()
{
	Game::GetInstance()->UnregisterObject(this);
	m_Handle = -1;
}

//////////////////////////////////////////////////////////////////////////
bool ScriptableObject::LoadFromXml(TiXmlElement* rootNode)
{
	for (TiXmlElement* elem = rootNode->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		if (elem->ValueStr() == "Name")
		{
			SetName(XmlUtil::TextToString(elem));
		}
		else if (elem->ValueStr() == "Scripts")
		{
			for (TiXmlElement* scriptNode = elem->FirstChildElement(); scriptNode != NULL; scriptNode = scriptNode->NextSiblingElement())
			{
				for (TiXmlElement* scriptPropNode = scriptNode->FirstChildElement(); scriptPropNode != NULL; scriptPropNode = scriptPropNode->NextSiblingElement())
				{
					if (scriptPropNode->ValueStr() == "File")
					{
						AddScript(XmlUtil::TextToString(scriptPropNode));
					}
				}
			}
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ScriptableObject::SaveToXml(TiXmlElement* rootNode)
{
	TiXmlElement* elem;

	elem = XmlUtil::AddElem("Name", rootNode);
	XmlUtil::SetText(elem, GetName());


	if (!m_Scripts.empty())
	{
		elem = XmlUtil::AddElem("Scripts", rootNode);
		foreach (ScriptPtr script, m_Scripts)
		{
			if (script->IsChildThread()) continue;

			TiXmlElement* scriptNode = XmlUtil::AddElem("Script", elem);

			TiXmlElement* fileNode = XmlUtil::AddElem("File", scriptNode);
			XmlUtil::SetFileName(fileNode, script->GetFileName());
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ScriptableObject::AddScript(const WideString& fileName)
{
	if (!CanAttachScripts()) return false;

	ScriptPtr newScript = Game::GetInstance()->GetScriptMgr()->RunScript(fileName, this);
	if (newScript)
	{
		m_Scripts.push_back(newScript);
		return true;
	}
	else return false;
}

//////////////////////////////////////////////////////////////////////////
bool ScriptableObject::AddScript(ScriptPtr script)
{
	if (!CanAttachScripts()) return false;

	m_Scripts.push_back(script);
	return true;
}

//////////////////////////////////////////////////////////////////////////
void ScriptableObject::RemoveScript(ScriptPtr script)
{
	if (!CanAttachScripts()) return;

	m_Scripts.remove(script);
}

//////////////////////////////////////////////////////////////////////////
void ScriptableObject::RemoveScript(const WideString& fileName)
{
	WideString normFileName = PathUtil::NormalizeFileName(fileName);

	Scripts toDelete;

	foreach (ScriptPtr script, m_Scripts)
	{
		WideString scriptFileName = PathUtil::NormalizeFileName(script->GetFileName());
		if (scriptFileName == normFileName) toDelete.push_back(script);
	}

	foreach (ScriptPtr script, toDelete)
	{
		RemoveScript(script);
	}
}

//////////////////////////////////////////////////////////////////////////
size_t ScriptableObject::HasScript(const WideString& fileName) const
{
	size_t numScripts = 0;

	WideString normFileName = PathUtil::NormalizeFileName(fileName);

	foreach (ScriptPtr script, m_Scripts)
	{
		WideString scriptFileName = PathUtil::NormalizeFileName(script->GetFileName());
		if (scriptFileName == normFileName) numScripts++;
	}
	return numScripts;
}

//////////////////////////////////////////////////////////////////////////
GameTier* ScriptableObject::GetTier()
{
	if (m_Tier) return m_Tier;
	else return Game::GetInstance()->GetDefaultTier();
}

//////////////////////////////////////////////////////////////////////////
ScriptableObject::PropertyAccessor* ScriptableObject::GetPropAccessor(const WideString& propName)
{
	ClassPropAccessors::iterator it;
	it = s_PropAccessors.find(GetClassID());
	if (it != s_PropAccessors.end())
	{
		PropAccessorCollection& accessors = s_PropAccessors[GetClassID()];
		PropAccessorCollection::iterator it;

		it = accessors.find(propName);
		if (it != accessors.end()) return &(*it).second;
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
Value* ScriptableObject::GetProperty(const WideString& name)
{
	// try native first
	PropertyAccessor* propAccessor = GetPropAccessor(name);
	if (propAccessor)
	{
		Value* ret = Game::GetInstance()->GetValueMgr()->CreateValue();

		// invoke getter
		if ((this->*propAccessor->GetHandler())(name, ret, false)) return ret;
	}

	// generic properties
	ValueCollection::iterator valIt;
	valIt = m_Properties.find(name);

	if (valIt != m_Properties.end()) return (*valIt).second;
	else
	{
		Value* nullVal = Game::GetInstance()->GetValueMgr()->CreateValue();
		return nullVal;
	}
}

//////////////////////////////////////////////////////////////////////////
bool ScriptableObject::HasNativeProperty(const WideString& name)
{
	PropertyAccessor* propAccessor = GetPropAccessor(name);
	if (propAccessor) return true;
	else return false;
}

//////////////////////////////////////////////////////////////////////////
void ScriptableObject::SetProperty(const WideString& name, Value* val)
{
	// try native first
	PropertyAccessor* propAccessor = GetPropAccessor(name);
	if (propAccessor)
	{
		if (propAccessor->IsReadOnly())
		{
			Game::GetInstance()->Log(L"Attempting to set read-only property '" + name + L"'. Ignored.");
			return;
		}

		// invoke setter
		if ((this->*propAccessor->GetHandler())(name, val, true)) return;
	}


	// generic properties
	if (CanSetGenericProps())
	{
		if (val == NULL || val->IsValueType())
		{
			Value* newVal = Game::GetInstance()->GetValueMgr()->CreateValue();
			if (val) newVal->SetValue(val);

			if (OnSetGenericProperty(name, newVal))
				m_Properties[name] = newVal;
		}
		else
		{
			if (OnSetGenericProperty(name, val))
			{
				m_Properties[name] = val;
				Game::GetInstance()->GetValueMgr()->WriteBarrier(NULL, val);
			}
		}
	}	
}

//////////////////////////////////////////////////////////////////////////
bool ScriptableObject::OnSetGenericProperty(const WideString& name, Value* val)
{
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ScriptableObject::HasProperty(const WideString& name)
{
	// try native first
	PropertyAccessor* propAccessor = GetPropAccessor(name);
	if (propAccessor) return true;


	// otherwise go for generic properties
	ValueCollection::const_iterator valIt;
	valIt = m_Properties.find(name);
	return (valIt != m_Properties.end());
}

//////////////////////////////////////////////////////////////////////////
bool ScriptableObject::DeleteProperty(const WideString& name)
{
	ValueCollection::iterator it;
	it = m_Properties.find(name);
	if (it != m_Properties.end())
	{
		m_Properties.erase(it);
		return true;
	}
	else return false;
}

//////////////////////////////////////////////////////////////////////////
bool ScriptableObject::HasNativeMethod(const WideString& name) const
{
	ClassMethods::const_iterator it;
	it = s_Methods.find(GetClassID());
	if (it != s_Methods.end())
	{
		MethodCollection& methods = s_Methods[GetClassID()];
		MethodCollection::const_iterator it;
		
		it = methods.find(name);
		return (it != methods.end());
	}
	else return false;
}

//////////////////////////////////////////////////////////////////////////
bool ScriptableObject::InvokeNativeMethod(Script* script, const WideString& methodName, bool async)
{
	ClassMethods::const_iterator it;
	it = s_Methods.find(GetClassID());
	if (it != s_Methods.end())
	{
		MethodCollection& methods = s_Methods[GetClassID()];
		MethodCollection::const_iterator it;

		it = methods.find(methodName);
		if (it != methods.end())
		{
			MethodHandler mh = (*it).second;
			return (this->*mh)(script, methodName, async);
		}
		else return false;
	}
	else return false;
}

//////////////////////////////////////////////////////////////////////////
bool ScriptableObject::HasScriptMethod(const WideString& name) const
{
	Scripts::const_iterator it;

	for (it = m_Scripts.begin(); it != m_Scripts.end(); it++)
	{
		if ((*it)->IsChildThread()) continue;

		if ((*it)->HasMethod(name)) return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool ScriptableObject::InvokeScriptMethod(Script* script, const WideString& methodName, bool async)
{
	// we're searching from the end; if multiple scripts contain the method, the last one wins
	reverse_foreach (ScriptPtr attachedScript, m_Scripts)
	{
		if (attachedScript->IsChildThread()) continue;
		
		if (attachedScript->HasMethod(methodName))
		{
			return attachedScript->CreateChildThread(script, methodName, this, async) != NULL;
		}		
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
Script* ScriptableObject::CreateEventHandlerThread(Script* script, const WideString& methodName)
{
	return script->CreateChildThread(NULL, methodName, this, true);
}

//////////////////////////////////////////////////////////////////////////
void ScriptableObject::GetScriptsWithMethod(const WideString& methodName, Scripts& scripts)
{
	scripts.clear();
	foreach (ScriptPtr script, m_Scripts)
	{
		if (script->IsChildThread()) continue;

		if (script->HasMethod(methodName)) scripts.push_back(script);
	}
}

//////////////////////////////////////////////////////////////////////////
void ScriptableObject::AddRef()
{
	if (m_IsRefCounted)	m_RefCount++;
}

//////////////////////////////////////////////////////////////////////////
void ScriptableObject::RemoveRef()
{
	if (m_IsRefCounted)
	{
		m_RefCount--;
		if (m_RefCount <= 0)
		{
			// garbage collector (de)registration
			delete this;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void ScriptableObject::InitializeLookupTables()
{
	RegisteredClasses::iterator it;
	it = s_RegisteredClasses.find(GetClassID());
	if (it != s_RegisteredClasses.end()) return;
	
	s_RegisteredClasses.insert(GetClassID());
	s_Methods[GetClassID()] = MethodCollection();
	s_PropAccessors[GetClassID()] = PropAccessorCollection();

	RegisterScriptInterface();	
}

//////////////////////////////////////////////////////////////////////////
void ScriptableObject::RegisterScriptMethod(WideString name, MethodHandler method)
{
	MethodCollection& methods = s_Methods[GetClassID()];
	methods[name] = method;
}

//////////////////////////////////////////////////////////////////////////
void ScriptableObject::RegisterScriptProp(WideString name, PropertyHandler handler, bool isReadOnly)
{
	if (handler)
	{
		PropAccessorCollection& accessors = s_PropAccessors[GetClassID()];
		accessors[name] = PropertyAccessor(handler, isReadOnly);
	}
}

//////////////////////////////////////////////////////////////////////////
void ScriptableObject::AddRootsToGrayList(ValueManager* valMgr)
{
	ValueCollection::const_iterator it;

	for (it = m_Properties.begin(); it != m_Properties.end(); it++)
	{
		valMgr->AddToGrayList((*it).second);
	}
}

//////////////////////////////////////////////////////////////////////////
WideStringListPtr ScriptableObject::GetMethodList() const
{
	WideStringListPtr list = WideStringListPtr(new WideStringList());

	ClassMethods::const_iterator it;
	it = s_Methods.find(GetClassID());
	if (it != s_Methods.end())
	{
		MethodCollection& methods = s_Methods[GetClassID()];
		MethodCollection::const_iterator it;

		for (it = methods.begin(); it != methods.end(); it++)
		{
			list->push_back((*it).first);
		}
	}
	return list;
}

//////////////////////////////////////////////////////////////////////////
WideStringListPtr ScriptableObject::GetPropList() const
{
	WideStringListPtr list = WideStringListPtr(new WideStringList());

	ClassPropAccessors::iterator it;
	it = s_PropAccessors.find(GetClassID());
	if (it != s_PropAccessors.end())
	{
		PropAccessorCollection& accessors = s_PropAccessors[GetClassID()];
		PropAccessorCollection::iterator it;

		for (it = accessors.begin(); it != accessors.end(); it++)
		{
			list->push_back((*it).first);
		}
	}
	return list;
}


//////////////////////////////////////////////////////////////////////////
// script interface
//////////////////////////////////////////////////////////////////////////
void ScriptableObject::RegisterScriptInterface()
{
	RegisterScriptMethod(L"Call",	&ScriptableObject::ScCall);
	RegisterScriptMethod(L"Spawn",	&ScriptableObject::ScSpawn);
	
	RegisterScriptProp(L"Name",	&ScriptableObject::ScPropName, false);
	RegisterScriptProp(L"Type",	&ScriptableObject::ScPropType, true);
}

//////////////////////////////////////////////////////////////////////////
bool ScriptableObject::ScCall(Script* script, const WideString& methodName, bool async)
{
	return DynamicCall(script, methodName, false);
}

//////////////////////////////////////////////////////////////////////////
bool ScriptableObject::ScSpawn(Script* script, const WideString& methodName, bool async)
{
	return DynamicCall(script, methodName, true);
}

//////////////////////////////////////////////////////////////////////////
bool ScriptableObject::DynamicCall(Script* script, const WideString& methodName, bool async)
{
	Value* val = script->GetStack()->GrabLastParam();
	if (val->IsNull())
	{		
		script->RuntimeError(methodName + L"() - no method name specified");
		script->GetStack()->KillParams();
		script->GetStack()->PushNull();
		return true;
	}
	WideString callMethodName = val->GetString();


	// try script method first
	bool success = false;
	if (this->HasScriptMethod(callMethodName))
	{
		success = this->InvokeScriptMethod(script, callMethodName, async);
	}

	// otherwise try native method
	if (!success && this->HasNativeMethod(callMethodName))
	{
		success = this->InvokeNativeMethod(script, callMethodName, async);
	}

	if (!success)
	{
		script->RuntimeError(methodName + L"() - error calling method '" + callMethodName + L"'");
		script->GetStack()->KillParams();
		script->GetStack()->PushNull();
	}
	else if (async)
	{
		script->GetStack()->Push(true);
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////
bool ScriptableObject::ScPropName(const WideString& prop, Value* val, bool isSetting)
{
	if (isSetting)
	{
		SetName(val->GetString());
		return true;
	}
	else
	{
		val->SetValue(GetName());
		return true;
	}
}


//////////////////////////////////////////////////////////////////////////
bool ScriptableObject::ScPropType(const WideString& prop, Value* val, bool isSetting)
{
	val->SetValue(this->GetTypeName());
	return true;
}


} // namespace Wme
