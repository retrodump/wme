// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeGameTier_H__
#define __WmeGameTier_H__


#include "Object.h"


namespace Wme
{
	class Timer;

	class WmeDllExport GameTier : public Object
	{
	public:

		enum TierType
		{
			TIER_LIVE,
			TIER_GAME,
			TIER_GUI
		};

		GameTier();
		~GameTier();

		void ProcessFrame();
		Timer* GetTimer() { return m_Timer; }

		unsigned long GetCurrentTime();

		void Pause();
		void Resume();
		bool IsPaused() { return m_IsPaused; }

	private:
		Timer* m_Timer;
		bool m_IsPaused;
	};
}
	
#endif // __WmeGameTier_H__
