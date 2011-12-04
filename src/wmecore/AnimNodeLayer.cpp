// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "AnimNodeLayer.h"
#include "AnimLayer.h"
#include "MeshEntity.h"
#include "Exception.h"
#include "StringUtil.h"
#include "AnimTree.h"
#include "Skeleton.h"
#include "Animation.h"
#include "AnimTreePin.h"
#include "AnimNodeAnimation.h"
#include "BoneMask.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
AnimNodeLayer::AnimNodeLayer(AnimTree* parentTree) : AnimTreeNode(parentTree)
{
}

//////////////////////////////////////////////////////////////////////////
AnimNodeLayer::~AnimNodeLayer()
{
	LayerMap::iterator it;
	for (it = m_Layers.begin(); it != m_Layers.end(); it++)
	{
		SAFE_DELETE(it->second);
	}
	m_Layers.clear();
}

//////////////////////////////////////////////////////////////////////////
void AnimNodeLayer::Create()
{

}

//////////////////////////////////////////////////////////////////////////
void AnimNodeLayer::ProcessBlending(float weight, AnimNodeList& animNodes)
{
	Skeleton* skeleton = m_ParentTree->GetOwner()->GetSkeleton();
	if (!skeleton) return;


	BoneWeights availableWeights, newAvailableWeights;
	availableWeights.Resize(skeleton->GetOgreSkeleton()->getNumBones(), weight);
	
	LayerMap::reverse_iterator it;
	for (it = m_Layers.rbegin(); it != m_Layers.rend(); it++)
	{
		AnimLayer* layer = it->second;

		// propagate blending
		layer->GetInputPin()->ProcessBlending(weight * layer->GetWeight(), animNodes);

		// assign appropriate weights to bones and update available weights
		newAvailableWeights.Copy(availableWeights);

		AnimNodeList activeAnims;
		layer->GetInputPin()->GetActiveAnimations(activeAnims);
		foreach (AnimNodeAnimation* animNode, activeAnims)
		{
			animNode->GetAnimation()->SetBoneWeights(availableWeights, newAvailableWeights);
		}
		availableWeights.Copy(newAvailableWeights);
	}
}

//////////////////////////////////////////////////////////////////////////
void AnimNodeLayer::GetActiveAnimations(AnimNodeList& animNodes)
{
	LayerMap::iterator it;

	for (it = m_Layers.begin(); it != m_Layers.end(); it++)
	{
		AnimLayer* layer = it->second;
		layer->GetInputPin()->GetActiveAnimations(animNodes);
	}
}

//////////////////////////////////////////////////////////////////////////
AnimLayer* AnimNodeLayer::GetLayer(size_t id)
{
	LayerMap::iterator it = m_Layers.find(id);
	if (it == m_Layers.end()) return NULL;
	else return it->second;
}

//////////////////////////////////////////////////////////////////////////
bool AnimNodeLayer::HasLayer(size_t id) const
{
	return (m_Layers.find(id) != m_Layers.end());
}

//////////////////////////////////////////////////////////////////////////
AnimLayer* AnimNodeLayer::AddLayer(size_t id, AnimLayer::LayerType type)
{
	AnimLayer* layer = GetLayer(id);
	if (layer != NULL) throw Exception(L"Animation layer " + StringUtil::ToString(id) + L" already exists.");

	layer = new AnimLayer(this, type);
	m_Layers[id] = layer;

	return layer;
}

//////////////////////////////////////////////////////////////////////////
void AnimNodeLayer::RemoveLayer(size_t id)
{
	LayerMap::iterator it = m_Layers.find(id);
	if (it == m_Layers.end()) return;

	SAFE_DELETE(it->second);
	m_Layers.erase(it);
}


} // namespace Wme