// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Game.h"
#include "AnimTree.h"
#include "MeshEntity.h"
#include "AnimNodeOutput.h"
#include "Timer.h"
#include "Animation.h"
#include "StringUtil.h"
#include "Exception.h"
#include "AnimTreePin.h"
#include "AnimNodeAnimation.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
AnimTree::AnimTree(MeshEntity* owner)
{
	m_Owner = owner;

	m_OutputNode = NULL;
}

//////////////////////////////////////////////////////////////////////////
AnimTree::~AnimTree()
{
	DeleteNodes();
	DeleteAnimations();
}

//////////////////////////////////////////////////////////////////////////
void AnimTree::Create()
{
	AddOutputNode();
}


//////////////////////////////////////////////////////////////////////////
void AnimTree::DeleteNode(AnimTreeNode* node)
{
	node->DeleteChildren();
	m_Nodes.remove(node);

	delete node;
}

//////////////////////////////////////////////////////////////////////////
void AnimTree::DeleteNodes()
{
	m_OutputNode = NULL;

	foreach (AnimTreeNode* node, m_Nodes)
	{
		SAFE_DELETE(node);
	}
	m_Nodes.clear();
}

//////////////////////////////////////////////////////////////////////////
void AnimTree::DeleteAnimations()
{
	AnimationMap::iterator it;
	for (it = m_Animations.begin(); it != m_Animations.end(); it++)
	{
		SAFE_DELETE(it->second);
	}
	m_Animations.clear();
}

//////////////////////////////////////////////////////////////////////////
void AnimTree::Update()
{
	if (!m_OutputNode) return;

	unsigned long elapsedTime = m_Owner->GetTier()->GetTimer()->GetElapsedTime();


	AnimationMap::iterator it;

	// reset animation weight
	for (it = m_Animations.begin(); it != m_Animations.end(); it++)
	{
		Animation* anim = it->second;
		anim->ResetAffectedBones();
		anim->ResetTotalWeight();
	}

	// propagate blending across the tree
	AnimNodeList finalAnimNodes;
	m_OutputNode->ProcessBlending(1.0f, finalAnimNodes);

	// set final animation weights
	for (it = m_Animations.begin(); it != m_Animations.end(); it++)
	{
		Animation* anim = it->second;

		if (Ogre::Math::RealEqual(anim->GetTotalWeight(), 0.0f))
		{
			anim->SetEnabled(false);
			anim->Update(elapsedTime);
		}
		else
		{
			anim->SetEnabled(true);
			anim->Update(elapsedTime);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
Animation* AnimTree::GetAnimation(const WideString& animName)
{
	Animation* anim;

	AnimationMap::iterator it = m_Animations.find(animName);
	if (it != m_Animations.end())
	{
		anim = m_Animations[animName];
	}
	else
	{
		Utf8String utf8Name = StringUtil::WideToUtf8(animName);

		Ogre::AnimationStateSet* animStateSet = m_Owner->GetOgreEntity()->getAllAnimationStates();
		if (animStateSet && animStateSet->hasAnimationState(utf8Name))
		{
			anim = new Animation(utf8Name, m_Owner);
			m_Animations[animName] = anim;
		}
		else
		{
			//Game::GetInstance()->Log(L"Animation '" + animName + L"' doesn't exist.");
			return NULL;
		}
	}
	return anim;
}

//////////////////////////////////////////////////////////////////////////
bool AnimTree::HasAnimation(const WideString& animName) const
{
	return m_Owner->HasAnimation(animName);
}

//////////////////////////////////////////////////////////////////////////
AnimNodeOutput* AnimTree::AddOutputNode()
{
	if (m_OutputNode) return m_OutputNode;

	m_OutputNode = new AnimNodeOutput(this);
	m_OutputNode->Create();
	m_Nodes.push_back(m_OutputNode);

	return m_OutputNode;
}

//////////////////////////////////////////////////////////////////////////
AnimNodeAnimation* AnimTree::AddAnimationNode(Animation* anim)
{
	AnimNodeAnimation* animNode = new AnimNodeAnimation(this);
	animNode->Create();
	animNode->SetAnimation(anim);
	m_Nodes.push_back(animNode);

	return animNode;
}


} // namespace Wme
