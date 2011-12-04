// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeDebugManager_H__
#define __WmeDebugManager_H__


#include "Object.h"


namespace Wme
{
	class DebugManager : public Object
	{
	public:
		DebugManager();
		~DebugManager();

		void ProcessFrame();
		void AddMessage(const WideString& msg);

		typedef std::list<WideString> QuickMessages;
		const QuickMessages& GetMessages() const { return m_Messages; }

	private:
		void PopMessage();

		QuickMessages m_Messages;
		size_t m_MaxMessages;
		dword m_Duration;

		typedef std::list<dword> StartTimes;
		StartTimes m_StartTimes;
	};
}


#endif // __WmeDebugManager_H__