// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeGenericProperty_H__
#define __WmeGenericProperty_H__


#include "Object.h"


namespace Wme
{
	class GenericProperty;
	class GenericPropertyValue;

	//////////////////////////////////////////////////////////////////////////
	typedef std::list<GenericProperty*> GenericPropertyList;
	typedef std::map<WideString, GenericPropertyValue*> GenericPropertyValues;

	//////////////////////////////////////////////////////////////////////////
	class WmeDllExport GenericPropertyValue : public Object
	{
	public:
		GenericPropertyValue() {};
		virtual ~GenericPropertyValue() {};

		virtual bool operator== (const GenericPropertyValue& a) const = 0;
		virtual GenericPropertyValue& operator= (const GenericPropertyValue& val) = 0;

		virtual WideString ToString() const = 0;

		virtual bool LoadFromXml(TiXmlElement* rootNode) = 0;
		virtual bool SaveToXml(TiXmlElement* rootNode) = 0;
	};

	//////////////////////////////////////////////////////////////////////////
	class WmeDllExport GenericProperty : public Object
	{
	public:
		GenericProperty();
		virtual ~GenericProperty();

		WideString GetName() const { return m_Name; }
		void SetName(const WideString& name) { m_Name = name; }

		WideString GetLabel() const { return m_Label.empty() ? m_Name : m_Label; }
		void SetLabel(const WideString& label) { m_Label = label; }

		WideString GetType() const { return m_Type; }

		GenericPropertyValue* GetDefaultValue() const { return m_DefaultValue; }
		void SetDefaultValue(GenericPropertyValue* value);

		bool IsDefaultValue(const GenericPropertyValue* value);

		virtual bool LoadFromXml(TiXmlElement* rootNode);
		virtual bool SaveToXml(TiXmlElement* rootNode);

		virtual GenericPropertyValue* CreateValue() = 0;


		// property factory
		static bool LoadPropertiesFromXml(TiXmlElement* propsNode, GenericPropertyList& properties);
		static bool SavePropertiesToXml(TiXmlElement* propsNode, const GenericPropertyList& properties);
		static GenericProperty* CreateProperty(const WideString& type);

	protected:
		WideString m_Name;
		WideString m_Label;
		WideString m_Type;

		GenericPropertyValue* m_DefaultValue;
	};


}

#endif // __WmeGenericProperty_H__
