// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "GameTier.h"
#include "Timer.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
GameTier::GameTier()
{
	m_Timer = new Timer();
	m_IsPaused = false;
}

//////////////////////////////////////////////////////////////////////////
GameTier::~GameTier()
{
	SAFE_DELETE(m_Timer);
}

//////////////////////////////////////////////////////////////////////////
void GameTier::ProcessFrame()
{
	m_Timer->Update();
}

//////////////////////////////////////////////////////////////////////////
unsigned long GameTier::GetCurrentTime()
{
	return m_Timer->GetTime();
}

//////////////////////////////////////////////////////////////////////////
void GameTier::Pause()
{
	if (m_IsPaused) return;

	m_Timer->Freeze();
	m_IsPaused = true;
}

//////////////////////////////////////////////////////////////////////////
void GameTier::Resume()
{
	if (!m_IsPaused) return;

	m_Timer->Unfreeze();
	m_IsPaused = false;
}


} // namespace Wme
