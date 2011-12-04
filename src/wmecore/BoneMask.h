// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeBoneMask_H__
#define __WmeBoneMask_H__


namespace Wme
{
	class WmeDllExport BoneMask
	{
	public:
		BoneMask(size_t initialSize = 0);
		~BoneMask();

		size_t GetSize() const { return m_Bones.size(); }

		bool GetValue(size_t index) const { return index < m_Bones.size() ? m_Bones[index] : false; }
		void SetValue(size_t index, bool val) { if (index < m_Bones.size()) m_Bones[index] = val; }

		void Resize(size_t newSize, bool initVal = false);
		void Clear(bool val = false);
		void Merge(const BoneMask* mask);
		void Subtract(const BoneMask* mask);
		void Copy(const BoneMask* mask);
		void Invert();

		bool HasAnyBone() const;

	private:
		typedef std::vector<bool> BoneVector;
		BoneVector m_Bones;
	};
}

#endif // __WmeBoneMask_H__
