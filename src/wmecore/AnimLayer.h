// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeAnimLayer_H__
#define __WmeAnimLayer_H__


namespace Wme
{
	class AnimTreeNode;
	class AnimTreePin;

	class AnimLayer
	{
	public:
		enum LayerType
		{
			LAYER_NORMAL,
			LAYER_ADDITIVE
		};

		AnimLayer(AnimTreeNode* ownerNode, LayerType type);
		~AnimLayer();

		LayerType GetType() const { return m_Type; }
		void SetType(LayerType type) { m_Type = type; }

		float GetWeight() const { return m_Weight; }
		void SetWeight(float weight) { m_Weight = weight; }

		AnimTreePin* GetInputPin() const { return m_InputPin; }

	private:
		AnimTreePin* m_InputPin;
		LayerType m_Type;
		float m_Weight;
	};
}

#endif // __WmeAnimLayer_H__
