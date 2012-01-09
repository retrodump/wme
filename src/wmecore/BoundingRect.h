// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeBoundingRect_H__
#define __WmeBoundingRect_H__

namespace Wme
{
	class WmeDllExport BoundingRect
	{
	public:
		BoundingRect();
		virtual ~BoundingRect();

		Ogre::Vector2 GetMinimum() const { return m_Minimum; }
		Ogre::Vector2 GetMaximum() const { return m_Maximum; }
		bool IsEmpty() const { return m_Empty; }
		void Clear();

		void AddPoint(float x, float y);
		void AddPoint(const Ogre::Vector2& point);

		bool ContainsPoint(float x, float y) const;
		bool ContainsPoint(const Ogre::Vector2& point) const;

	private:
		bool m_Empty;
		Ogre::Vector2 m_Minimum;
		Ogre::Vector2 m_Maximum;
	};
}

#endif // __WmeBoundingRect_H__