// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "SimpleAnimTree.h"
#include "AnimNodeLayer.h"
#include "AnimNodeInput.h"
#include "AnimNodeOutput.h"
#include "AnimNodeAnimation.h"
#include "AnimTreePin.h"
#include "Animation.h"

namespace Wme
{


//////////////////////////////////////////////////////////////////////////
SimpleAnimTree::SimpleAnimTree(MeshEntity* owner) : AnimTree(owner)
{	
	m_LayerNode = NULL;
}

//////////////////////////////////////////////////////////////////////////
SimpleAnimTree::~SimpleAnimTree()
{
}

//////////////////////////////////////////////////////////////////////////
void SimpleAnimTree::Create()
{
	AnimTree::Create();

	m_LayerNode = new AnimNodeLayer(this);
	m_LayerNode->Create();
	m_Nodes.push_back(m_LayerNode);

	m_OutputNode->GetInputPin(0)->SetConnection(m_LayerNode);
}

//////////////////////////////////////////////////////////////////////////
AnimNodeInput* SimpleAnimTree::GetLayerInputNode(AnimLayer* layer, bool forceCreate)
{
	AnimTreeNode* node = layer->GetInputPin()->GetConnection();
	if (!node || !node->IsInputNode())
	{
		if (!forceCreate) return NULL;

		if (node) DeleteNode(node);

		node = new AnimNodeInput(this);
		m_Nodes.push_back(node);

		layer->GetInputPin()->SetConnection(node);
	}
	return static_cast<AnimNodeInput*>(node);
}

//////////////////////////////////////////////////////////////////////////
void SimpleAnimTree::StartAnimation(size_t layer, const WideString& animName, bool crossFade, unsigned long fadeTime, bool animWhileFadingIn, bool animWhileFadingOut)
{
	AnimLayer* animLayer = m_LayerNode->GetLayer(layer);
	if (!animLayer)
	{
		animLayer = m_LayerNode->AddLayer(layer);		
	}

	AnimNodeInput* inputNode = GetLayerInputNode(animLayer, true);
	if (!inputNode) return;

	if (crossFade) inputNode->CrossFade(animName, fadeTime, animWhileFadingIn, animWhileFadingOut);
	else inputNode->Play(animName, fadeTime, animWhileFadingIn);
}



//////////////////////////////////////////////////////////////////////////
void SimpleAnimTree::StopAnimation(const WideString& animName, unsigned long fadeTime, bool animWhileFading)
{
	AnimNodeList animNodes;
	m_LayerNode->GetActiveAnimations(animNodes);

	foreach (AnimNodeAnimation* animNode, animNodes)
	{
		Animation* anim = animNode->GetAnimation();
		if (anim && anim->GetName() == animName)
		{
			anim->Stop(fadeTime, animWhileFading);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
bool SimpleAnimTree::IsAnimationPlaying(size_t layer, const WideString& animName, bool includeFadingIn)
{
	AnimLayer* animLayer = m_LayerNode->GetLayer(layer);
	if (!animLayer) return false;

	AnimNodeList animNodes;
	animLayer->GetInputPin()->GetActiveAnimations(animNodes);

	foreach (AnimNodeAnimation* animNode, animNodes)
	{
		Animation* anim = animNode->GetAnimation();
		if (anim && anim->GetName() == animName)
		{
			switch (anim->GetPlaybackState())
			{
			case Animation::STATE_PLAYING:
				return true;
			case Animation::STATE_FADING_IN:
				return includeFadingIn ? true : false;
			case Animation::STATE_FADING_OUT:
				return false;
			default:
				return false;
			}
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool SimpleAnimTree::IsAnyAnimationPlaying()
{
	AnimNodeList animNodes;
	m_LayerNode->GetActiveAnimations(animNodes);

	foreach (AnimNodeAnimation* animNode, animNodes)
	{
		Animation* anim = animNode->GetAnimation();

		switch (anim->GetPlaybackState())
		{
		case Animation::STATE_PLAYING:
		case Animation::STATE_FADING_IN:
		case Animation::STATE_FADING_OUT:
				return true;
		}
	}
	return false;
}


} // namespace Wme