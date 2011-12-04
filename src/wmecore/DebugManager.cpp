// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "DebugManager.h"
#include "Game.h"
#include "GameTier.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
DebugManager::DebugManager()
{
	m_MaxMessages = 5;
	m_Duration = 3000;
}

//////////////////////////////////////////////////////////////////////////
DebugManager::~DebugManager()
{
}

//////////////////////////////////////////////////////////////////////////
void DebugManager::ProcessFrame()
{
	dword currentTime = Game::GetInstance()->GetLiveTier()->GetCurrentTime();

	while (m_Messages.size() > 0)
	{
		dword firstMsgTime = m_StartTimes.front();
		if (currentTime - firstMsgTime >= m_Duration) PopMessage();
		else break;
	}
}

//////////////////////////////////////////////////////////////////////////
void DebugManager::AddMessage(const WideString& msg)
{
	while (m_Messages.size() >= m_MaxMessages) PopMessage();

	m_Messages.push_back(msg);
	m_StartTimes.push_back(Game::GetInstance()->GetLiveTier()->GetCurrentTime());
}

//////////////////////////////////////////////////////////////////////////
void DebugManager::PopMessage()
{
	if (m_Messages.size() > 0)
	{
		m_Messages.pop_front();
		m_StartTimes.pop_front();
	}
}


} // namespace Wme
