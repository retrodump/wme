// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeAnimNodeBlend_H__
#define __WmeAnimNodeBlend_H__


#include "AnimTreeNode.h"


namespace Wme
{
	class AnimTreePin;

	class AnimNodeBlend : public AnimTreeNode
	{
	public:
		AnimNodeBlend(AnimTree* parentTree);
		~AnimNodeBlend();

		void Create();
		void ProcessBlending(float weight, AnimNodeList& animNodes);
		void GetActiveAnimations(AnimNodeList& animNodes);

		void SetBlendRatio(float ratio) { m_BlendRatio = ratio; }
		float GetBlendRatio() const { return m_BlendRatio; }

	private:
		AnimTreePin* m_PinA;
		AnimTreePin* m_PinB;

		float m_BlendRatio; // 0.0 = NodeA, 1.0 = NodeB
	};
}

#endif // __WmeAnimNodeBlend_H__
