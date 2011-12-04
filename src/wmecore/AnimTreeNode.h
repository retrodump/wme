// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeAnimTreeNode_H__
#define __WmeAnimTreeNode_H__


#include "Object.h"
#include "AnimNodeList.h"


namespace Wme
{
	class AnimTree;
	class AnimTreePin;

	class AnimTreeNode : public Object
	{
	public:
		AnimTreeNode(AnimTree* parentTree);
		virtual ~AnimTreeNode();

		virtual void Create() = 0;

		const WideString& GetName() const { return m_Name; }
		void SetName(const WideString& name) { m_Name = name; }

		AnimTreePin* AddInputPin(const WideString& name);
		AnimTreePin* GetInputPin(const WideString& name);
		AnimTreePin* GetInputPin(int id);
		void RemoveInputPin(AnimTreePin* pin);

		void DeleteChildren();

		virtual void ProcessBlending(float weight, AnimNodeList& animNodes) {};
		virtual void GetActiveAnimations(AnimNodeList& animNodes) {};

		virtual bool IsInputNode() const { return false; }

	protected:
		AnimTree* m_ParentTree;

		typedef std::vector<AnimTreePin*> PinVector;
		PinVector m_InputPins;

	private:
		WideString m_Name;

		typedef std::map<WideString, int> PinNameMap;		
		PinNameMap m_PinNameMap;
	};
}

#endif // __WmeAnimTreeNode_H__
