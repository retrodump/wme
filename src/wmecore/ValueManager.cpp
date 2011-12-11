// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "ValueManager.h"
#include "Game.h"
#include "ScriptManager.h"
#include "StringUtil.h"

namespace Wme
{

const float ValueManager::HARD_TO_SOFT_LIMIT_RATIO = 0.9f;

//////////////////////////////////////////////////////////////////////////
ValueManager::ValueManager()
{
	m_FirstRun = true;
	m_GCState = GC_IDLE;

	m_NumToTrackPerFrame = 200;
	m_NumToDeletePerFrame = 200;

	m_MaxLimitHard = 1000;
	m_MaxLimitSoft = (size_t)((float)m_MaxLimitHard * HARD_TO_SOFT_LIMIT_RATIO);
	m_LimitGrowthRate = 1.2f;

	m_FramesSinceLastCollect = 0;
	m_GrowthSinceLastCollect = 0;
	m_PeakNumValues = 0;

	m_InitialPoolSize = 100;
	m_MaxPoolSize = 1000;
	
	InitPool();
}

//////////////////////////////////////////////////////////////////////////
ValueManager::~ValueManager()
{
	foreach (Value* val, m_Values)
	{
		SAFE_DELETE(val);
	}
	m_Values.clear();

	foreach (Value* val, m_ValuePool)
	{
		SAFE_DELETE(val);
	}
	m_ValuePool.clear();
}

//////////////////////////////////////////////////////////////////////////
void ValueManager::InitPool()
{
	while (m_ValuePool.size() < m_InitialPoolSize)
	{
		m_ValuePool.push_back(new Value);
	}
}

//////////////////////////////////////////////////////////////////////////
Value* ValueManager::CreateValue()
{
	m_GrowthSinceLastCollect++;

	Value* val;
	
	if (!m_ValuePool.empty())
	{
		val = m_ValuePool.front();
		m_ValuePool.pop_front();
	}
	else
	{
		val = new Value();
	}

	val->SetGCMark(Value::MARK_GRAY);
	m_Values.push_back(val);

	if (m_Values.size() > m_PeakNumValues) m_PeakNumValues = m_Values.size();

	return val;
}

//////////////////////////////////////////////////////////////////////////
void ValueManager::CollectGarbage()
{
	Game::GetInstance()->m_TestInfo = GetInfo();

	// we exceeded maximum allowed size
	if (m_Values.size() >= m_MaxLimitHard)
	{
		CollectFull();
		return;
	}


	if (m_GCState == GC_IDLE)
	{
		m_FramesSinceLastCollect++;
				
		// should we collect the garbage now?
		if (m_Values.size() >= m_MaxLimitSoft)
		{
			m_GCState = GC_TRACKING;
			m_FirstRun = true;
		}
	}


	if (m_GCState == GC_TRACKING)
	{
		if (m_FirstRun)
		{
			MarkAll(Value::MARK_WHITE);
			AddRootsToGrayList();
			m_FirstRun = false;
		}
		else
		{
			if (!TrackValues())
			{
#ifdef WME_GC_WRITE_BARRIER_CHECKING
				WriteBarrierCheck();
#endif
				m_GCState = GC_DELETING;
			}
		}
	}
	else if (m_GCState == GC_DELETING)
	{
		if (!DeleteValues())
		{
			m_GCState = GC_IDLE;

			m_FramesSinceLastCollect = 0;
			m_GrowthSinceLastCollect = 0;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void ValueManager::CollectFull()
{
	// prepare
	m_GCState = GC_TRACKING;
	MarkAll(Value::MARK_WHITE);
	AddRootsToGrayList();

	// track all
	while (TrackValues()) {};

	// delete all
	m_GCState = GC_DELETING;
	while (DeleteValues()) {};

	// finalize
	m_GCState = GC_IDLE;
	m_FramesSinceLastCollect = 0;
	m_GrowthSinceLastCollect = 0;

	AdjustLimits();
}

//////////////////////////////////////////////////////////////////////////
void ValueManager::AddRootsToGrayList()
{
	Game::GetInstance()->AddRootsToGrayList(this);
}

//////////////////////////////////////////////////////////////////////////
void ValueManager::MarkAll(Value::GCMarkType type)
{
	foreach (Value* val, m_Values)
	{
		val->SetGCMark(type);
	}
}

//////////////////////////////////////////////////////////////////////////
void ValueManager::AddToGrayList(Value* val)
{
	if (!val) return;

	// we're only interested while tracking
	if (m_GCState != GC_TRACKING) return;

	// only add unprocessed values to the list
	if (val->GetGCMark() == Value::MARK_WHITE)
	{
		val->SetGCMark(Value::MARK_GRAY);
		m_GrayList.push_back(val);
	}
}

//////////////////////////////////////////////////////////////////////////
bool ValueManager::TrackValues()
{
	int toTrack = m_NumToTrackPerFrame;
	while (toTrack > 0)
	{
		if (m_GrayList.size() == 0) return false;

		Value* valToTrack = m_GrayList.front();
		valToTrack->AddReferencesToGrayList(this);
		valToTrack->SetGCMark(Value::MARK_BLACK);

		m_GrayList.pop_front();
		toTrack--;
	}
	return true;
}


//////////////////////////////////////////////////////////////////////////
bool ValueManager::DeleteValues()
{
	int toDelete = m_NumToDeletePerFrame;

	Value::ValueList::iterator it;
	for (it = m_Values.begin(); it != m_Values.end(); it++)
	{
		Value* val = (*it);

		if (val->GetGCMark() == Value::MARK_WHITE)
		{
			if (m_ValuePool.size() < m_MaxPoolSize)
			{
				val->Clear();
				m_ValuePool.push_back(val);
			}
			else
			{
				delete val;
			}

			it = m_Values.erase(it);

			if (it != m_Values.begin() && it != m_Values.end()) it--;

			toDelete--;
			if (toDelete <= 0)
			{
				//Game::GetInstance()->Log(L"1 - Values left: " + StringUtil::ToString(m_Values.size()));
				return true;
			}

			if (it == m_Values.end()) break;
		}
	}
	//Game::GetInstance()->Log(L"2 - Values left: " + StringUtil::ToString(m_Values.size()));
	return false;
}

//////////////////////////////////////////////////////////////////////////
void ValueManager::WriteBarrier(Value* parentVal, Value* newVal)
{
	if (m_GCState != GC_TRACKING) return;

	// parentVal == NULL means newVal is becoming a root value
	if ((parentVal == NULL || parentVal->GetGCMark() == Value::MARK_BLACK) && newVal->GetGCMark() == Value::MARK_WHITE)
		AddToGrayList(newVal);
}


#ifdef WME_GC_WRITE_BARRIER_CHECKING
//////////////////////////////////////////////////////////////////////////
void ValueManager::WriteBarrierCheck()
{
	bool errorFound = false;

	foreach (Value* val, m_Values)
	{
		if (val->GetGCMark() == Value::MARK_BLACK && val->HasWhiteReferences())
		{
			errorFound = true;
			break;
		}
	}

	if (errorFound) Game::GetInstance()->Log(L"ValueManager::WriteBarrierCheck() - missing write barriers detected.");
}
#endif

//////////////////////////////////////////////////////////////////////////
void ValueManager::AdjustLimits()
{
	if (m_Values.size() >= m_MaxLimitHard)
	{
		m_MaxLimitHard = (size_t)((float)m_Values.size() * m_LimitGrowthRate);
		m_MaxLimitSoft = (size_t)((float)m_MaxLimitHard * HARD_TO_SOFT_LIMIT_RATIO);
	}
}

//////////////////////////////////////////////////////////////////////////
float ValueManager::GetGrowthRate() const
{
	if (m_GrowthSinceLastCollect == 0) return 0.0f;
	else return 1.0f + (float)m_Values.size() / (float)m_GrowthSinceLastCollect;
}

//////////////////////////////////////////////////////////////////////////
WideString ValueManager::GetInfo() const
{
	WideString str = L"GC: ";
	switch (m_GCState)
	{
	case GC_IDLE:
		str += L"idle";
		break;
	case GC_TRACKING:
		str += L"marking";
		break;
	case GC_DELETING:
		str += L"sweeping";
		break;
	}

	str += L"  values: " + StringUtil::ToString(m_Values.size());
	str += L"  peak: " + StringUtil::ToString(m_PeakNumValues);
	str += L"  pool: " + StringUtil::ToString(m_ValuePool.size());
	str += L"  limit: " + StringUtil::ToString(m_MaxLimitSoft) + L"/" + StringUtil::ToString(m_MaxLimitHard);

	return str;
}


} // namespace Wme
