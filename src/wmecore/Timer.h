// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeTimer_H__
#define __WmeTimer_H__

#include "Object.h"

class Ogre::Timer;

namespace Wme
{
	class WmeDllExport Timer : public Object
	{
	public:
		Timer();
		virtual ~Timer();		
		unsigned long Update();
		void Freeze();
		void Unfreeze();
		void Reset();

		unsigned long GetTime()
		{
			return m_CurrentTime;
		};

		unsigned long GetElapsedTime()
		{
			return m_ElapsedTime;
		}

		void SetTime(unsigned long val)
		{
			m_CurrentTime = val;
			m_PrevTime = val;
		};

		void SetMultiplier(float val)
		{
			m_Multiplier = val;
		}

		float GetMultiplier()
		{
			return m_Multiplier;
		};

		bool IsFrozen()
		{
			return m_IsFrozen;
		};

	private:
		Ogre::Timer* m_OgreTimer;
		bool m_IsFrozen;
		float m_Multiplier;
		unsigned long m_CurrentTime;
		unsigned long m_ElapsedTime;
		unsigned long m_PrevTime;

	};
}

#endif // __WmeTimer_H__
