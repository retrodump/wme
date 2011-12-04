// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Skeleton.h"
#include "StringUtil.h"
#include "Game.h"
#include "Renderer.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
Skeleton::Skeleton()
{
}

//////////////////////////////////////////////////////////////////////////
Skeleton::~Skeleton()
{
	m_OgreSkeleton.setNull();
	Game::GetInstance()->GetRenderer()->ScheduleSkeletonCleanup();

	foreach (SkeletonChange* change, m_ChangeList)
	{
		SAFE_DELETE(change);
	}
	m_ChangeList.clear();
}

//////////////////////////////////////////////////////////////////////////
bool Skeleton::LoadFromFile(const WideString& fileName)
{
	if (fileName.empty()) return false;

	Utf8String skeletonName = StringUtil::WideToUtf8(fileName);

	try
	{
		m_OgreSkeleton = Ogre::SkeletonManager::getSingleton().load(skeletonName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	}
	catch (Ogre::Exception& e)
	{
		m_OgreSkeleton.setNull();
		Game::GetInstance()->Log(L"Error loading skeleton '" + fileName + L"'.");
		Game::GetInstance()->Log(e.getDescription());
		return false;
	}

	m_OgreSkeleton->setBlendMode(Ogre::ANIMBLEND_CUMULATIVE);
	m_FileName = fileName;

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool Skeleton::AddSkeleton(const WideString& fileName, const WideString& animsToMerge, bool recordChanges)
{
	if (fileName.empty()) return false;

	Utf8String skeletonName = StringUtil::WideToUtf8(fileName);
	Ogre::SkeletonPtr newSkeleton;

	try
	{
		newSkeleton = Ogre::SkeletonManager::getSingleton().load(skeletonName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	}
	catch (Ogre::Exception& e)
	{
		Game::GetInstance()->Log(L"Error loading skeleton '" + fileName + L"'.");
		Game::GetInstance()->Log(e.getDescription());
		return false;
	}

	Ogre::Skeleton::BoneHandleMap boneHandleMap;
	newSkeleton->_buildMapBoneByHandle(newSkeleton.getPointer(), boneHandleMap);

	WideStringSet animSet;
	GetSkeletonAnimList(newSkeleton, animsToMerge, animSet);
	Ogre::StringVector animList;

	foreach (WideString str, animSet)
	{
		animList.push_back(StringUtil::WideToUtf8(StringUtil::Trim(str)));
	}


	try
	{
		m_OgreSkeleton->_mergeSkeletonAnimations(newSkeleton.getPointer(), boneHandleMap, animList);
	}
	catch (Ogre::Exception& e)
	{
		Game::GetInstance()->Log(L"Error merging skeleton '" + fileName + L"'.");
		Game::GetInstance()->Log(e.getDescription());
		return false;
	}

	// record changes
	if (recordChanges)
	{
		m_ChangeList.push_back(new SkeletonChange(fileName, animSet));
	}
	

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool Skeleton::DeleteAnimation(const WideString& animationName, bool recordChanges)
{
	Utf8String animName = StringUtil::WideToUtf8(animationName);
	if (!m_OgreSkeleton->hasAnimation(animName)) return false;

	m_OgreSkeleton->removeAnimation(animName);

	
	// record change
	if (recordChanges)
	{
		bool found = false;
		reverse_foreach (SkeletonChange* change, m_ChangeList)
		{
			if (change->DeleteAnim(animationName))
			{
				found = true;
				if (change->IsEmpty())
				{
					m_ChangeList.remove(change);
					delete change;
				}
				break;
			}
		}
		if (!found) m_DeleteFromBase.insert(animationName);
	}
	
	return true;
}

//////////////////////////////////////////////////////////////////////////
void Skeleton::ReplayChanges()
{
	LoadFromFile(m_FileName);

	foreach (WideString animName, m_DeleteFromBase)
	{
		DeleteAnimation(animName, false);
	}

	foreach (SkeletonChange* change, m_ChangeList)
	{
		AddSkeleton(change->GetFileName(), change->GetAnimList(), false);
	}
}

//////////////////////////////////////////////////////////////////////////
void Skeleton::GetSkeletonAnimList(Ogre::SkeletonPtr skeleton, const WideString& reqList, WideStringSet& animSet)
{
	WideStringListPtr requiredAnims = StringUtil::Split(reqList, L",");
	std::set<WideString> reqSet;

	foreach (WideString str, *requiredAnims.get())
	{
		if (reqSet.find(str) == reqSet.end())
			reqSet.insert(str);
	}


	word numAnims = skeleton->getNumAnimations();
	for (word i = 0; i < numAnims; i++)
	{
		WideString animName = StringUtil::Utf8ToWide(skeleton->getAnimation(i)->getName());
		if (reqSet.empty() || reqSet.find(animName) != reqSet.end())
		{
			animSet.insert(animName);
		}
	}
}


} // namespace Wme
