// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeResizableElement2D_H__
#define __WmeResizableElement2D_H__


#include "Element2D.h"


namespace Wme
{
	class SpriteTexture;

	class WmeDllExport ResizableElement2D : public Element2D
	{
	public:
		ResizableElement2D();
		virtual ~ResizableElement2D();

		void AddGeometry();

		float GetWidth() const { return m_Width; }
		void SetWidth(float val);

		float GetHeight() const { return m_Height; }
		void SetHeight(float val);

		SpriteTexture* GetTexture() const { return m_Texture; }
		void SetTexture(SpriteTexture* val);

		Ogre::ColourValue GetColor() const { return m_Color; }
		void SetColor(const Ogre::ColourValue& val);

		float GetFrameTopHeight() const { return m_FrameTopHeight; }
		void SetFrameTopHeight(float val);

		float GetFrameBottomHeight() const { return m_FrameBottomHeight; }
		void SetFrameBottomHeight(float val);

		float GetFrameLeftWidth() const { return m_FrameLeftWidth; }
		void SetFrameLeftWidth(float val);

		float GetFrameRightWidth() const { return m_FrameRightWidth; }
		void SetFrameRightWidth(float val);

		bool GetStretchVertical() const { return m_StretchVertical; }
		void SetStretchVertical(bool val);

		bool GetStretchHorizontal() const { return m_StretchHorizontal; }
		void SetStretchHorizontal(bool val);

		bool GetPaintMiddlePart() const { return m_PaintMiddlePart; }
		void SetPaintMiddlePart(bool val);


	private:
		enum TileType
		{
			TILE_TOP_LEFT,
			TILE_TOP_MIDDLE,
			TILE_TOP_RIGHT,
			TILE_MIDDLE_LEFT,
			TILE_MIDDLE_MIDDLE,
			TILE_MIDDLE_RIGHT,
			TILE_BOTTOM_LEFT,
			TILE_BOTTOM_MIDDLE,
			TILE_BOTTOM_RIGHT
		};

		float m_Width;
		float m_Height;
		SpriteTexture* m_Texture;

		Ogre::ColourValue m_Color;

		float m_FrameTopHeight;
		float m_FrameBottomHeight;
		float m_FrameLeftWidth;
		float m_FrameRightWidth;

		bool m_StretchHorizontal;
		bool m_StretchVertical;
		bool m_PaintMiddlePart;

		void GetNumTiles(int& numTiles, int& tilesHorizontal, int& tilesVertical);
		void AddTile(float row, float col, TileType tileType, float width, float height, Vertex2DTex* vertBuffer, int& vertexOffset);
		void GetTextureCoordinates(TileType type, Ogre::FloatRect& rect);

	};
}

#endif // __WmeResizableElement2D_H__