// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeSimpleAnimTree_H__
#define __WmeSimpleAnimTree_H__


#include "AnimTree.h"


namespace Wme
{
	class AnimNodeLayer;
	class AnimNodeInput;
	class AnimLayer;

	class WmeDllExport SimpleAnimTree : public AnimTree
	{
	public:
		SimpleAnimTree(MeshEntity* owner);
		~SimpleAnimTree();

		void Create();

		void StartAnimation(size_t layer, const WideString& animName, bool crossFade, unsigned long fadeTime, bool animWhileFadingIn, bool animWhileFadingOut);
		void StopAnimation(const WideString& animName, unsigned long fadeTime, bool animWhileFading);
		bool IsAnimationPlaying(size_t layer, const WideString& animName, bool includeFadingIn = true);
		bool IsAnyAnimationPlaying();

	private:
		AnimNodeInput* GetLayerInputNode(AnimLayer* layer, bool forceCreate = false);

		AnimNodeLayer* m_LayerNode;
	};
}

#endif // __WmeSimpleAnimTree_H__

