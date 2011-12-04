// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeAnimTree_H__
#define __WmeAnimTree_H__


#include "Object.h"
#include "AnimNodeList.h"


namespace Wme
{
	class MeshEntity;
	class AnimTreeNode;
	class AnimNodeOutput;
	class Animation;


	class AnimTree : public Object
	{
	public:
		AnimTree(MeshEntity* owner);
		virtual ~AnimTree();

		virtual void Create();
		void Update();


		Animation* GetAnimation(const WideString& animName);
		bool HasAnimation(const WideString& animName) const;


		MeshEntity* GetOwner() const { return m_Owner; }

		AnimNodeOutput* AddOutputNode();
		AnimNodeAnimation* AddAnimationNode(Animation* anim);
		void DeleteNode(AnimTreeNode* node);

	protected:
		typedef std::list<AnimTreeNode*> NodeList;
		NodeList m_Nodes;
		MeshEntity* m_Owner;
		AnimNodeOutput* m_OutputNode;

	private:
		void DeleteNodes();
		void DeleteAnimations();

		typedef std::map<WideString, Animation*> AnimationMap;
		AnimationMap m_Animations;
	};
}

#endif // __WmeAnimTree_H__
