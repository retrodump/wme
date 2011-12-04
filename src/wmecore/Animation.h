// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeAnimation_H__
#define __WmeAnimation_H__


#include "Object.h"
#include "BoneMask.h"
#include "BoneWeights.h"


namespace Wme
{
	class MeshEntity;

	class WmeDllExport Animation : public Object
	{
	public:
		Animation(const Utf8String& animName, MeshEntity* owner);
		~Animation();

		enum PlaybackMode
		{
			PLAY_SINGLE,
			PLAY_LOOP,
			PLAY_SINGLE_FREEZE
		};

		enum PlaybackState
		{
			STATE_STOPPED,
			STATE_PLAYING,
			STATE_FADING_IN,
			STATE_FADING_OUT,
			STATE_FINISHED
		};

		
		void ResetTotalWeight();
		void IncreaseTotalWeight(float weightIncrement);
		float GetTotalWeight() const { return m_TotalWeight; }

		void SetWeight();
		void SetEnabled(bool enabled);
		bool GetEnabled() const;

		void SetPlaybackRate(float rate) { m_PlaybackRate = rate; }
		float GetPlaybackRate() const { return m_PlaybackRate; }

		void Play(unsigned long fadeInTime = 0, bool animateWhileFading = false);
		void Stop(unsigned long fadeOutTime = 0, bool animateWhileFading = false);
		void Update(unsigned long elapsedTime);

		void Reset();

		bool IsFinished() const;

		void MarkUsedBones(BoneMask& usedBones);

		void ResetAffectedBones();
		void MaskAffectedBones(BoneMask& boneMask);
		bool HasAffectedBones() const;

		void SetBoneWeights(const BoneWeights& availableWeights, BoneWeights& newAvailableWeights);

		PlaybackMode GetPlaybackMode() const { return m_PlaybackMode; }
		void SetPlaybackMode(PlaybackMode mode);
		
		bool GetRootMovement() const { return m_RootMovement; }
		void SetRootMovement(bool rootMovement);

		void SetFadeOutTime(unsigned long fadeOutTime);

		PlaybackState GetPlaybackState() const { return m_PlaybackState; }

		WideString GetName() const;

	private:
		Ogre::AnimationState* GetAnimState() const;

		float m_TotalWeight;
		float m_PlaybackRate;

		bool m_RootMovement;

		PlaybackState m_PlaybackState;
		PlaybackMode m_PlaybackMode;

		unsigned long m_FadeInTime;
		unsigned long m_FadeOutTime;
		bool m_AnimateWhileFading;
		unsigned long m_FadeStartTime;

		Utf8String m_AnimName;
		MeshEntity* m_Owner;

		void GetUsedBones();
		BoneMask m_UsedBones;
		BoneMask m_AffectedBones;
	};
}

#endif // __WmeAnimation_H__