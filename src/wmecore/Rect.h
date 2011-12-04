// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeRect_H__
#define __WmeRect_H__


namespace Wme
{
	class Rect
	{
	public:
		Rect();
		Rect(float x, float y, float width, float height);
		~Rect();

		Rect(const Rect& r);
		Rect& operator= (const Rect& r);
		bool operator!= (const Rect& r) const;
		bool operator== (const Rect& r) const;

		float x;
		float y;
		float width;
		float height;

		float GetLeft() const { return x; }
		float GetRight() const { return x + width; }
		float GetTop() const { return y; }
		float GetBottom() const { return y + height; }

		void Clone(const Rect& r);
		bool IsEmpty() const { return *this == EMPTY_RECT; }
		Rect GetIntersection(const Rect& r) const;
		bool Intersects(const Rect& r) const;
		void UnionWith(const Rect& r);

		static const Rect EMPTY_RECT;
	};
}

#endif // __WmeRect_H__
