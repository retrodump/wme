// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmePropertyFloat_H__
#define __WmePropertyFloat_H__


#include "GenericProperty.h"


namespace Wme
{
	class PropertyFloat;

	//////////////////////////////////////////////////////////////////////////
	class PropertyValueFloat : public GenericPropertyValue
	{
	public:
		PropertyValueFloat(PropertyFloat* parentProp);
		~PropertyValueFloat();

		float GetValue() const { return m_Value; }
		void SetValue(float value) { m_Value = value; }

		bool operator== (const GenericPropertyValue& val) const;
		GenericPropertyValue& operator= (const GenericPropertyValue& val);

		WideString ToString() const;

		bool LoadFromXml(TiXmlElement* rootNode);
		bool SaveToXml(TiXmlElement* rootNode);

		PropertyFloat* GetParentProp() const { return m_ParentProp; }

	private:
		PropertyFloat* m_ParentProp;
		float m_Value;
	};

	//////////////////////////////////////////////////////////////////////////
	class PropertyFloat : public GenericProperty
	{
	public:
		PropertyFloat()
		{
			m_Type = L"float";
			m_CheckRange = false;
			m_MinValue = m_MaxValue = 0.0f;
			m_Decimals = 2;
		}
		~PropertyFloat() {};
		

		bool LoadFromXml(TiXmlElement* rootNode);
		bool SaveToXml(TiXmlElement* rootNode);

		bool GetCheckRange() const { return m_CheckRange; }
		float GetMinValue() const { return m_MinValue; }
		float GetMaxValue() const { return m_MaxValue; }
		size_t GetDecimals() const { return m_Decimals; }
		

		GenericPropertyValue* CreateValue() { return new PropertyValueFloat(this); }

	private:
		bool m_CheckRange;
		float m_MinValue;
		float m_MaxValue;
		size_t m_Decimals;
	};
}


#endif // __WmePropertyFloat_H__