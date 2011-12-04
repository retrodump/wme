// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeBoneWeights_H__
#define __WmeBoneWeights_H__


namespace Wme
{
	class WmeDllExport BoneWeights
	{
	public:
		BoneWeights(size_t initialSize = 0);
		~BoneWeights();

		size_t GetSize() const { return m_BoneWeights.size(); }

		float GetValue(size_t index) const { return index < m_BoneWeights.size() ? m_BoneWeights[index] : false; }
		void SetValue(size_t index, float val) { if (index < m_BoneWeights.size()) m_BoneWeights[index] = val; }

		void Resize(size_t newSize, float initVal = 1.0f);
		void Fill(float val = 1.0f);

		void Copy(const BoneWeights& mask);


	private:
		typedef std::vector<float> BoneWeightVector;
		BoneWeightVector m_BoneWeights;

	};
}

#endif // __WmeBoneWeights_H__
