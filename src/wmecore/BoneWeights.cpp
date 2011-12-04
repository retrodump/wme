// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "BoneWeights.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
BoneWeights::BoneWeights(size_t initialSize)
{
	if (initialSize > 0) Resize(initialSize);
}

//////////////////////////////////////////////////////////////////////////
BoneWeights::~BoneWeights()
{
}

//////////////////////////////////////////////////////////////////////////
void BoneWeights::Resize(size_t newSize, float initVal)
{
	m_BoneWeights.resize(newSize, initVal);
}

//////////////////////////////////////////////////////////////////////////
void BoneWeights::Fill(float val)
{
	std::fill(m_BoneWeights.begin(), m_BoneWeights.end(), val);
}

//////////////////////////////////////////////////////////////////////////
void BoneWeights::Copy(const BoneWeights& weights)
{
	Resize(weights.GetSize());

	for (size_t i = 0; i < m_BoneWeights.size(); i++)
	{
		m_BoneWeights[i] = weights.GetValue(i);
	}
}


} // namespace Wme
