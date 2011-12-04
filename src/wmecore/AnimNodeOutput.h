// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeAnimNodeOutput_H__
#define __WmeAnimNodeOutput_H__


#include "AnimTreeNode.h"


namespace Wme
{
	class AnimTreePin;

	class AnimNodeOutput : public AnimTreeNode
	{
	public:
		AnimNodeOutput(AnimTree* parentTree);
		~AnimNodeOutput();

		void Create();
		void ProcessBlending(float weight, AnimNodeList& animNodes);
		void GetActiveAnimations(AnimNodeList& animNodes);

	private:
		AnimTreePin* m_InputPin;
	};
}

#endif // __WmeAnimNodeOutput_H__
