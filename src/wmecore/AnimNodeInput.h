// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeAnimNodeInput_H__
#define __WmeAnimNodeInput_H__


#include "AnimTreeNode.h"


namespace Wme
{
	class WmeDllExport AnimNodeInput : public AnimTreeNode
	{
	public:
		AnimNodeInput(AnimTree* parentTree);
		virtual ~AnimNodeInput();

		void Create();
		void ProcessBlending(float weight, AnimNodeList& animNodes);
		void GetActiveAnimations(AnimNodeList& animNodes);

		void Play(const WideString& animName, unsigned long fadeInTime = 0, bool animateWhileFading = false);
		void CrossFade(const WideString& animName, unsigned long fadeInTime = 0, bool animateWhileFadingIn = false, bool animateWhileFadingOut = false);

		bool IsInputNode() const { return true; }


	private:
		PinVector m_ActiveAnims;
		AnimTreePin* ClaimPin(const WideString& name);
		void RemoveActivePin(AnimTreePin* activePin);


	};
}

#endif // __WmeAnimNodeInput_H__
