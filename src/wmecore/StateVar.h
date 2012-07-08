// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeStateVar_H__
#define __WmeStateVar_H__


namespace Wme
{
	template<typename Type>
	class WmeDllExport StateVar
	{
	public:
		StateVar() {}

		StateVar(Type init)
		{
			m_DefaultValue = init;
		}

		~StateVar() {}

		Type GetValue() const { return m_DefaultValue; }
		void SetValue(Type value) { m_DefaultValue = value; }

		//////////////////////////////////////////////////////////////////////////
		Type GetValue(const WideString& stateName) const
		{
			if (stateName.length() == 0) return m_DefaultValue;

			ValueMap::const_iterator it = m_Values.find(stateName);
			if (it == m_Values.end()) return m_DefaultValue;
			else return it->second;
		}

		//////////////////////////////////////////////////////////////////////////
		void SetValue(Type value, const WideString& stateName)
		{
			if (stateName.length() == 0) m_DefaultValue = value;
			else m_Values[stateName] = value;
		}

		//////////////////////////////////////////////////////////////////////////
		void DeleteAllValues()
		{
			delete m_DefaultValue;
			m_DefaultValue = NULL;

			for (ValueMap::iterator it = m_Values.begin(); it != m_Values.end(); ++it)
			{
				delete it->second;
			}
			m_Values.clear();
		}

		//////////////////////////////////////////////////////////////////////////
		void DeleteValue(const WideString& stateName)
		{
			if (stateName.length() == 0)
			{
				delete m_DefaultValue;
				m_DefaultValue = NULL;
			}
			else
			{
				if (m_Values.find(stateName) != m_Values.end())
				{
					delete m_Values[stateName];
					m_Values[stateName] = NULL;
				}
			}
		}

		//////////////////////////////////////////////////////////////////////////
		void RemoveState(const WideString stateName)
		{
			if (m_Values.find(stateName) != m_Values.end()) m_Values.erase(stateName);
		}

		//////////////////////////////////////////////////////////////////////////
		void GetStates(std::set<WideString>& states)
		{
			for (ValueMap::iterator it = m_Values.begin(); it != m_Values.end(); ++it)
			{
				if (states.find(it->first) == states.end()) states.insert(it->first);
			}
		}

		//////////////////////////////////////////////////////////////////////////
		bool HasState(const WideString& stateName)
		{
			return m_Values.find(stateName) != m_Values.end();
		}

	private:
		Type m_DefaultValue;

		typedef std::map<WideString, Type> ValueMap;
		ValueMap m_Values;

	};
}

#endif // __WmeStateVar_H__
