// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeInteractiveElement2D_H__
#define __WmeInteractiveElement2D_H__


#include "Element2D.h"


namespace Wme
{
	class WmeDllExport InteractiveElement2D : public Element2D
	{
	public:
		InteractiveElement2D();
		virtual ~InteractiveElement2D();

		float GetWidth() const { return m_Width; }
		void SetWidth(float val);

		float GetHeight() const { return m_Height; }
		void SetHeight(float val);

		void SetSize(float width, float height);

		void AddGeometry();
		bool IsTransparentAt(float x, float y);

	private:
		float m_Width;
		float m_Height;
	};
}

#endif // __WmeInteractiveElement2D_H__