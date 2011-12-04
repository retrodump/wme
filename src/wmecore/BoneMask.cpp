// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "BoneMask.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
BoneMask::BoneMask(size_t initialSize)
{
	if (initialSize > 0) Resize(initialSize);
}

//////////////////////////////////////////////////////////////////////////
BoneMask::~BoneMask()
{
}

//////////////////////////////////////////////////////////////////////////
void BoneMask::Resize(size_t newSize, bool initVal)
{
	m_Bones.resize(newSize, initVal);
}

//////////////////////////////////////////////////////////////////////////
void BoneMask::Clear(bool val)
{
	std::fill(m_Bones.begin(), m_Bones.end(), val);
}

//////////////////////////////////////////////////////////////////////////
void BoneMask::Merge(const BoneMask* mask)
{
	for (size_t i = 0; i < m_Bones.size(); i++)
	{
		if (mask->GetValue(i)) m_Bones[i] = true;
	}
}

//////////////////////////////////////////////////////////////////////////
void BoneMask::Subtract(const BoneMask* mask)
{
	for (size_t i = 0; i < m_Bones.size(); i++)
	{
		if (mask->GetValue(i)) m_Bones[i] = false;
	}
}

//////////////////////////////////////////////////////////////////////////
void BoneMask::Copy(const BoneMask* mask)
{
	Resize(mask->GetSize());
	for (size_t i = 0; i < m_Bones.size(); i++)
	{
		m_Bones[i] = mask->GetValue(i);
	}
}

//////////////////////////////////////////////////////////////////////////
void BoneMask::Invert()
{
	for (size_t i = 0; i < m_Bones.size(); i++)
	{
		m_Bones[i] = !m_Bones[i];
	}
}

//////////////////////////////////////////////////////////////////////////
bool BoneMask::HasAnyBone() const
{
	for (size_t i = 0; i < m_Bones.size(); i++)
	{
		if (m_Bones[i]) return true;
	}
	return false;
}


} // namespace Wme