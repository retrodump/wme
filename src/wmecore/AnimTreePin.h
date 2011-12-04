// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeAnimTreePin_H__
#define __WmeAnimTreePin_H__


#include "Object.h"
#include "AnimNodeList.h"


namespace Wme
{
	class AnimTreeNode;

	class AnimTreePin : public Object
	{
	public:
		AnimTreePin(const WideString& name, AnimTreeNode* ownerNode);
		~AnimTreePin();

		const WideString& GetName() const { return m_Name; }

		const AnimTreeNode* GetOwnerNode() const { return m_OwnerNode; }

		AnimTreeNode* GetConnection() const { return m_ConnectedNode; }
		void SetConnection(AnimTreeNode* node) { m_ConnectedNode = node; }
		bool HasConnection() const { return m_ConnectedNode != NULL; }

		void ProcessBlending(float weight, AnimNodeList& animNodes);
		void GetActiveAnimations(AnimNodeList& animNodes);

	private:
		WideString m_Name;

		AnimTreeNode* m_OwnerNode;
		AnimTreeNode* m_ConnectedNode;

	};
}

#endif // __WmeAnimTreePin_H__
