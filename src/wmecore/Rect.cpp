// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Rect.h"


namespace Wme
{


const Rect Rect::EMPTY_RECT(0, 0, 0, 0);

//////////////////////////////////////////////////////////////////////////
Rect::Rect()
{
	x = y = width = height = 0;
}

//////////////////////////////////////////////////////////////////////////
Rect::Rect(float x, float y, float width, float height)
{
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
}

//////////////////////////////////////////////////////////////////////////
Rect::Rect(const Rect& r)
{
	Clone(r);
}

//////////////////////////////////////////////////////////////////////////
Rect& Rect::operator= (const Rect& r)
{
	if (this == &r) return *this;
	Clone(r);
	return *this;
}

//////////////////////////////////////////////////////////////////////////
bool Rect::operator!= (const Rect& r) const
{
	return x != r.x || y != r.y || width != r.width || height != r.height;
}

//////////////////////////////////////////////////////////////////////////
bool Rect::operator== (const Rect& r) const
{
	return x == r.x && y == r.y && width == r.width && height == r.height;
}

//////////////////////////////////////////////////////////////////////////
Rect::~Rect()
{
}

//////////////////////////////////////////////////////////////////////////
void Rect::Clone(const Rect& r)
{
	x = r.x;
	y = r.y;
	width = r.width;
	height = r.height;
}

//////////////////////////////////////////////////////////////////////////
bool Rect::IsEmpty() const
{
	return x == 0 && y == 0 && width == 0 && height == 0;
}

//////////////////////////////////////////////////////////////////////////
Rect Rect::GetIntersection(const Rect& r) const
{
	Rect ret;

	if (Intersects(r))
	{
		ret.x = std::max(x, r.x);
		ret.y = std::max(y, r.y);
		ret.width = std::min(x + width, r.x + r.width) - ret.x;
		ret.height = std::min(y + height, r.y + r.height) - ret.y;
	}

	return ret;
}

//////////////////////////////////////////////////////////////////////////
bool Rect::Intersects(const Rect& r) const
{
	if ((x > (r.x + r.width)) || ((x + width) < r.x))
		return false;

	if ((y > (r.y + r.height)) || ((y + height) < r.y))
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////
void Rect::UnionWith(const Rect& r)
{
	width = std::max(GetRight(), r.GetRight()) - x;
	height = std::max(GetBottom(), r.GetBottom()) - y;

	x = std::min(x, r.x);
	y = std::min(y, r.y);	
}

//////////////////////////////////////////////////////////////////////////
bool Rect::ContainsPoint(float testX, float testY) const
{
	return testX >= x && testX <= x + width && testY >= y && testY <= y + height;
}

//////////////////////////////////////////////////////////////////////////
void Rect::Normalize()
{
	if (width < 0)
	{
		x += width;
		width = Ogre::Math::Abs(width);
	}
	
	if (height < 0)
	{
		y += height;
		height = Ogre::Math::Abs(height);
	}
}


} // namespace Wme
