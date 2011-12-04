// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeScriptableObject_H__
#define __WmeScriptableObject_H__

#include "Object.h"
#include "Script.h"

namespace Wme
{
	class Value;
	class ValueManager;
	class GameTier;

	class WmeDllExport ScriptableObject : public Object
	{
	public:
		typedef bool (ScriptableObject::*MethodHandler)(Script* script, const WideString& methodName, bool async);
		typedef bool (ScriptableObject::*PropertyHandler)(const WideString& prop, Value* val, bool isSetting);

		ScriptableObject();
		virtual ~ScriptableObject();
		
		int GetHandle()
		{
			return m_Handle;
		};

		int Register();
		void Unregister();

		bool AddScript(const WideString& fileName);
		bool AddScript(ScriptPtr script);
		void RemoveScript(const WideString& fileName);
		void RemoveScript(ScriptPtr script);
		size_t HasScript(const WideString& fileName) const;

		WideString GetName() const { return m_Name; }
		virtual void SetName(const WideString& name) { m_Name = name; }

		Value* GetProperty(const WideString& name);
		bool HasNativeProperty(const WideString& name);
		void SetProperty(const WideString& name, Value* val);
		bool HasProperty(const WideString& name);
		bool DeleteProperty(const WideString& name);
		bool HasNativeMethod(const WideString& name) const;
		bool HasScriptMethod(const WideString& name) const;
		bool InvokeNativeMethod(Script* script, const WideString& methodName, bool async);
		bool InvokeScriptMethod(Script* script, const WideString& methodName, bool async);
		Script* CreateEventHandlerThread(Script* script, const WideString& methodName);

		void GetScriptsWithMethod(const WideString& methodName, Scripts& scripts);

		virtual bool OnSetGenericProperty(const WideString& name, Value* val);

		WideStringListPtr GetMethodList() const;
		WideStringListPtr GetPropList() const;
		
		virtual int Compare(const ScriptableObject* val) const
		{
			if (val == this) return 0;
			else if (this < val) return -1;
			else return 1;
		};

		virtual WideString GetTypeName() const { return L"ScriptableObject"; }

		virtual int ConvertToInt() { return 0; }
		virtual double ConvertToDouble() { return 0.0; }
		virtual bool ConvertToBool() { return true; }
		virtual WideString ConvertToString() { return L"Native object"; }

		virtual bool IsReady()
		{
			return true;
		};

		virtual void AddRef();
		virtual void RemoveRef();

		virtual const WideString GetClassID() const = 0;

		void AddRootsToGrayList(ValueManager* valMgr);

		GameTier* GetTier();
		void SetTier(GameTier* tier) { m_Tier = tier; }

		bool LoadFromXml(TiXmlElement* rootNode);
		bool SaveToXml(TiXmlElement* rootNode);

		bool CanAttachScripts() const { return m_CanAttachScripts; }
		void SetCanAttachScripts(bool val) { m_CanAttachScripts = val; }

		bool CanSetGenericProps() const { return m_CanSetGenericProps; }
		void SetCanSetGenericProps(bool val) { m_CanSetGenericProps = val; }

	protected:
		int m_Handle;
		Scripts m_Scripts;

		int m_RefCount;
		bool m_IsRefCounted;

		WideString m_Name;

		bool m_CanAttachScripts;
		bool m_CanSetGenericProps;

		void ShutdownScripts();

		
		typedef std::map<WideString, Value*> ValueCollection;
		ValueCollection m_Properties;

		virtual void RegisterScriptInterface();
		void RegisterScriptMethod(WideString name, MethodHandler method);
		void RegisterScriptProp(WideString name, PropertyHandler handler, bool isReadOnly);

		//////////////////////////////////////////////////////////////////////////
		// script interface
		//////////////////////////////////////////////////////////////////////////
		bool ScCall(Script* script, const WideString& methodName, bool async);
		bool ScSpawn(Script* script, const WideString& methodName, bool async);

		bool ScPropName(const WideString& prop, Value* val, bool isSetting);
		bool ScPropType(const WideString& prop, Value* val, bool isSetting);


	private:
		//////////////////////////////////////////////////////////////////////////
		class WmeDllExport PropertyAccessor
		{
		public:
			PropertyAccessor()
			{
				m_Handler = NULL;
				m_IsReadOnly = true;
			}

			PropertyAccessor(PropertyHandler handler, bool isReadOnly)
			{
				m_Handler = handler;
				m_IsReadOnly = isReadOnly;
			}

			PropertyHandler GetHandler() const
			{
				return m_Handler;
			}

			bool IsReadOnly() const
			{
				return m_IsReadOnly;
			}

		private:
			PropertyHandler m_Handler;
			bool m_IsReadOnly;
		};


		typedef std::map<WideString, MethodHandler> MethodCollection;
		typedef std::map<WideString, PropertyAccessor> PropAccessorCollection;

		typedef std::map<WideString, MethodCollection> ClassMethods;
		typedef std::map<WideString, PropAccessorCollection> ClassPropAccessors;
		typedef std::set<WideString> RegisteredClasses;

		static ClassMethods s_Methods;
		static ClassPropAccessors s_PropAccessors;
		static RegisteredClasses s_RegisteredClasses;

		void InitializeLookupTables();
		bool DynamicCall(Script* script, const WideString& methodName, bool async);

		PropertyAccessor* GetPropAccessor(const WideString& propName);

		GameTier* m_Tier;
	};
}


#endif // __WmeScriptableObject_H__