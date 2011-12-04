// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "SkeletonManager.h"
#include "Skeleton.h"
#include "PathUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
SkeletonManager::SkeletonManager()
{
}

//////////////////////////////////////////////////////////////////////////
SkeletonManager::~SkeletonManager()
{
	foreach (SkeletonMap::value_type val, m_Skeletons)
	{
		SAFE_DELETE(val.second);
	}
	m_Skeletons.clear();
}

//////////////////////////////////////////////////////////////////////////
bool SkeletonManager::Initialize()
{
	return true;
}

//////////////////////////////////////////////////////////////////////////
Skeleton* SkeletonManager::GetSkeleton(const WideString& fileName)
{
	WideString safeFileName = PathUtil::NormalizeFileName(fileName);

	SkeletonMap::iterator it;
	it = m_Skeletons.find(safeFileName);

	if (it != m_Skeletons.end())
	{
		(*it).second->AddRef();
		return (*it).second;
	}
	else
	{
		Skeleton* skeleton = new Skeleton();
		if (skeleton->LoadFromFile(safeFileName))
		{
			m_Skeletons[safeFileName] = skeleton;
			skeleton->AddRef();
			return skeleton;
		}
		else
		{
			delete skeleton;
			return NULL;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void SkeletonManager::ReleaseSkeleton(Skeleton*& skeleton)
{
	if (!skeleton) return;

	SkeletonMap::iterator it;
	it = m_Skeletons.find(PathUtil::NormalizeFileName(skeleton->GetFileName()));

	if (it != m_Skeletons.end())
	{
		if (!it->second->ReleaseRef()) m_Skeletons.erase(it);
	}
	skeleton = NULL;
}


} // namespace Wme
