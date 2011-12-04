// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeAnimNodeLayer_H__
#define __WmeAnimNodeLayer_H__


#include "AnimTreeNode.h"
#include "AnimLayer.h"


namespace Wme
{
	//////////////////////////////////////////////////////////////////////////
	class WmeDllExport AnimNodeLayer : public AnimTreeNode
	{
	public:
		
		AnimNodeLayer(AnimTree* parentTree);
		~AnimNodeLayer();

		void Create();
		void ProcessBlending(float weight, AnimNodeList& animNodes);
		void GetActiveAnimations(AnimNodeList& animNodes);

		AnimLayer* GetLayer(size_t id);
		AnimLayer* AddLayer(size_t id, AnimLayer::LayerType type = AnimLayer::LAYER_NORMAL);
		void RemoveLayer(size_t id);
		bool HasLayer(size_t id) const;

	private:
		typedef std::map<size_t, AnimLayer*> LayerMap;
		LayerMap m_Layers;
	};
}

#endif // __WmeAnimNodeLayer_H__
