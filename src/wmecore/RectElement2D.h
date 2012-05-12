// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeRectElement2D_H__
#define __WmeRectElement2D_H__


#include "Element2D.h"


namespace Wme
{
	class WmeDllExport RectElement2D : public Element2D
	{
	public:
		RectElement2D();
		virtual ~RectElement2D();

		void AddGeometry();

		float GetWidth() const { return m_Width; }
		void SetWidth(float val);

		float GetHeight() const { return m_Height; }
		void SetHeight(float val);

		float GetStrokeThickness() const { return m_StrokeThickness; }
		void SetStrokeThickness(float val);

		bool IsFilled() const { return m_Filled; }
		void SetFilled(bool val);

		Ogre::ColourValue GetFillColor() const { return m_FillColor; }
		void SetFillColor(const Ogre::ColourValue& val);

		Ogre::ColourValue GetStrokeColor() const { return m_StrokeColor; }
		void SetStrokeColor(const Ogre::ColourValue& val);

	private:
		float m_Width;
		float m_Height;
		float m_StrokeThickness;
		bool m_Filled;
		Ogre::ColourValue m_FillColor;
		Ogre::ColourValue m_StrokeColor;

		Ogre::MaterialPtr m_Material;

		void AddRect(float x, float y, float width, float height, const Ogre::ColourValue& color, Vertex2D* vertBuffer, int& vertexOffset);
	};
}

#endif // __WmeRectElement2D_H__