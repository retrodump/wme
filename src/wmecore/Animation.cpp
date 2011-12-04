// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Animation.h"
#include "MeshEntity.h"
#include "Skeleton.h"
#include "Timer.h"
#include "StringUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
Animation::Animation(const Utf8String& animName, MeshEntity* owner)
{
	m_AnimName = animName;
	m_Owner = owner;

	m_PlaybackState = STATE_STOPPED;

	Ogre::AnimationState* animState = GetAnimState();
	m_PlaybackMode = (animState && animState->getLoop()) ? PLAY_LOOP : PLAY_SINGLE;

	m_FadeInTime = m_FadeOutTime = 0;
	m_AnimateWhileFading = false;
	m_FadeStartTime = 0;

	m_TotalWeight = 0.0f;

	m_PlaybackRate = 1.0f;

	m_RootMovement = false;

	GetUsedBones();
}

//////////////////////////////////////////////////////////////////////////
Animation::~Animation()
{
	Ogre::AnimationState* animState = GetAnimState();
	if (animState) animState->setEnabled(false);
}

//////////////////////////////////////////////////////////////////////////
Ogre::AnimationState* Animation::GetAnimState() const
{
	Ogre::Entity* ent = m_Owner->GetOgreEntity();
	if (ent && ent->getAllAnimationStates()->hasAnimationState(m_AnimName)) return ent->getAnimationState(m_AnimName);
	else return NULL;
}

//////////////////////////////////////////////////////////////////////////
WideString Animation::GetName() const
{
	return StringUtil::Utf8ToWide(m_AnimName);
}

//////////////////////////////////////////////////////////////////////////
void Animation::ResetTotalWeight()
{
	m_TotalWeight = 0.0f;
}

//////////////////////////////////////////////////////////////////////////
void Animation::IncreaseTotalWeight(float weightIncrement)
{
	m_TotalWeight += weightIncrement;
}

//////////////////////////////////////////////////////////////////////////
void Animation::SetWeight()
{
	Ogre::AnimationState* animState = GetAnimState();
	if (!animState) return;

	animState->setWeight(m_TotalWeight);

	// create blend mask if it doesn't exist yet
	if (!animState->hasBlendMask())
	{
		animState->createBlendMask(m_AffectedBones.GetSize());
	}

	// set bone weights according to m_AffectBones
	for (size_t i = 0; i < m_AffectedBones.GetSize(); i++)
	{
		animState->setBlendMaskEntry(i, m_AffectedBones.GetValue(i) ? m_TotalWeight : 0.0f);
	}
}

//////////////////////////////////////////////////////////////////////////
void Animation::SetEnabled(bool enabled)
{
	Ogre::AnimationState* animState = GetAnimState();
	if (animState) animState->setEnabled(enabled);
}

//////////////////////////////////////////////////////////////////////////
bool Animation::GetEnabled() const
{
	Ogre::AnimationState* animState = GetAnimState();
	if (animState) return animState->getEnabled();
	else return false;
}

//////////////////////////////////////////////////////////////////////////
void Animation::SetPlaybackMode(PlaybackMode mode)
{
	Ogre::AnimationState* animState = GetAnimState();
	if (!animState) return;

	m_PlaybackMode = mode;
	if (m_PlaybackMode == PLAY_LOOP) animState->setLoop(true);
	else animState->setLoop(false);
}

//////////////////////////////////////////////////////////////////////////
void Animation::Reset()
{
	m_PlaybackState = STATE_STOPPED;
	m_PlaybackRate = 1.0f;

	Ogre::AnimationState* animState = GetAnimState();
	if (animState) animState->setTimePosition(0.0f);
}

//////////////////////////////////////////////////////////////////////////
void Animation::Play(unsigned long fadeInTime, bool animateWhileFading)
{
	Ogre::AnimationState* animState = GetAnimState();
	if (!animState) return;

	animState->setTimePosition(0.0f);
	if (fadeInTime > 0)
	{
		m_FadeInTime = fadeInTime;
		m_AnimateWhileFading = animateWhileFading;
		m_PlaybackState = STATE_FADING_IN;
		m_FadeStartTime = m_Owner->GetTier()->GetTimer()->GetTime();
	}
	else m_PlaybackState = STATE_PLAYING;
}

//////////////////////////////////////////////////////////////////////////
void Animation::Stop(unsigned long fadeOutTime, bool animateWhileFading)
{
	if (IsFinished()) return;

	if (fadeOutTime > 0)
	{
		m_FadeOutTime = fadeOutTime;
		m_AnimateWhileFading = animateWhileFading;
		m_PlaybackState = STATE_FADING_OUT;
		m_FadeStartTime = m_Owner->GetTier()->GetTimer()->GetTime();
	}
	else m_PlaybackState = STATE_FINISHED;
}

//////////////////////////////////////////////////////////////////////////
void Animation::SetFadeOutTime(unsigned long fadeOutTime)
{
	m_FadeOutTime = fadeOutTime;
}

//////////////////////////////////////////////////////////////////////////
void Animation::Update(unsigned long elapsedTime)
{
	Ogre::AnimationState* animState = GetAnimState();
	if (!animState) return;

	unsigned long currentTime = m_Owner->GetTier()->GetTimer()->GetTime();

	// add time to animation
	if (m_PlaybackState == STATE_PLAYING || ((m_PlaybackState == STATE_FADING_OUT || m_PlaybackState == STATE_FADING_OUT) && m_AnimateWhileFading))
	{
		if (!animState->hasEnded())
		{
			float timeSeconds = (float)elapsedTime / 1000.0f;
			animState->addTime(timeSeconds * m_PlaybackRate);
		}
	}

	// finished playback?
	if (m_PlaybackState == STATE_PLAYING && animState->hasEnded() && m_PlaybackMode != PLAY_SINGLE_FREEZE)
	{
		if (m_FadeOutTime > 0)
		{
			m_PlaybackState = STATE_FADING_OUT;
			m_FadeStartTime = currentTime;
		}
		else m_PlaybackState = STATE_FINISHED;
	}

	// faded out?
	if (m_PlaybackState == STATE_FADING_OUT && (currentTime - m_FadeStartTime) >= m_FadeOutTime)
	{
		m_PlaybackState = STATE_FINISHED;
	}

	// faded in?
	if (m_PlaybackState == STATE_FADING_IN && (currentTime - m_FadeStartTime) >= m_FadeInTime)
	{
		m_PlaybackState = STATE_PLAYING;
	}


	// TODO events

}

//////////////////////////////////////////////////////////////////////////
bool Animation::IsFinished() const
{
	if (m_PlaybackState == STATE_FINISHED) return true;
	else return false;
}

//////////////////////////////////////////////////////////////////////////
void Animation::MarkUsedBones(BoneMask& usedBones)
{
	usedBones.Merge(&m_UsedBones);
}

//////////////////////////////////////////////////////////////////////////
void Animation::GetUsedBones()
{
	Ogre::AnimationState* animState = GetAnimState();
	if (!animState) return;

	Ogre::SkeletonPtr skeleton = m_Owner->GetOgreEntity()->getMesh()->getSkeleton();
	if (skeleton.isNull()) return;

	if (!skeleton->hasAnimation(animState->getAnimationName())) return;

	m_UsedBones.Resize(skeleton->getNumBones());

	Ogre::Animation* anim = skeleton->getAnimation(animState->getAnimationName());

	// walk through all tracks of this animation and mark their bones as used
	Ogre::Animation::NodeTrackIterator trackIt = anim->getNodeTrackIterator();
	while (trackIt.hasMoreElements())
	{
		Ogre::NodeAnimationTrack* track = trackIt.getNext();	

		Ogre::Bone* bone = (Ogre::Bone*)track->getAssociatedNode();
		unsigned short boneId = bone->getHandle();

		m_UsedBones.SetValue(boneId, true);
	}
}

//////////////////////////////////////////////////////////////////////////
void Animation::ResetAffectedBones()
{
	m_AffectedBones.Copy(&m_UsedBones);
}

//////////////////////////////////////////////////////////////////////////
void Animation::MaskAffectedBones(BoneMask& boneMask)
{
	m_AffectedBones.Subtract(&boneMask);
}

//////////////////////////////////////////////////////////////////////////
bool Animation::HasAffectedBones() const
{
	return m_AffectedBones.HasAnyBone();
}

//////////////////////////////////////////////////////////////////////////
void Animation::SetBoneWeights(const BoneWeights& availableWeights, BoneWeights& newAvailableWeights)
{
	Ogre::AnimationState* animState = GetAnimState();
	if (!animState) return;

	unsigned long currentTime = m_Owner->GetTier()->GetTimer()->GetTime();

	// create blend mask if it doesn't exist yet
	if (!animState->hasBlendMask())
	{
		animState->createBlendMask(m_UsedBones.GetSize());
	}


	for (size_t i = 0; i < m_UsedBones.GetSize(); i++)
	{
		float weight = 0.0f;

		if (m_UsedBones.GetValue(i))
		{
			weight = GetTotalWeight() * availableWeights.GetValue(i);

			// if we are fading in/out, lower the weight appropriately
			if (m_PlaybackState == STATE_FADING_IN)
			{
				weight = weight * ((float)(currentTime - m_FadeStartTime) / (float)m_FadeInTime);
			}
			else if (m_PlaybackState == STATE_FADING_OUT)
			{
				weight = weight * (1.0f - (float)(currentTime - m_FadeStartTime) / (float)m_FadeOutTime);
			}
			weight = Ogre::Math::Clamp(weight, 0.0f, 1.0f);

			newAvailableWeights.SetValue(i, std::max(0.0f, newAvailableWeights.GetValue(i) - weight));
		}
		
		animState->setBlendMaskEntry(i, weight);
	}
}

//////////////////////////////////////////////////////////////////////////
void Animation::SetRootMovement(bool rootMovement)
{
	m_RootMovement = rootMovement;

	if (rootMovement)
	{
		//Ogre::SkeletonPtr skeleton = m_Owner->GetOgreSkeleton();
		//if (skeleton.isNull()) return;
		Ogre::SkeletonInstance* skeleton = m_Owner->GetOgreEntity()->getSkeleton();
		if (skeleton)
		{
			Ogre::Bone* rootBone = skeleton->getRootBone();
			rootBone->removeChild(rootBone->getChild(0));
		}
	}
}



} // namespace Wme
