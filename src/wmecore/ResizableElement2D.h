// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeResizableElement2D_H__
#define __WmeResizableElement2D_H__


#include "Element2D.h"
#include "ResizableImage.h"


namespace Wme
{
	class WmeDllExport ResizableElement2D : public Element2D, public ResizableImage::Listener
	{
	public:
		ResizableElement2D();
		virtual ~ResizableElement2D();

		void AddGeometry();

		float GetWidth() const { return m_Width; }
		void SetWidth(float val);

		float GetHeight() const { return m_Height; }
		void SetHeight(float val);

		void SetSize(float width, float height);

		ResizableImage* GetImage() const { return m_Image; }
		void SetImage(ResizableImage* val);

		Ogre::ColourValue GetColor() const { return m_Color; }
		void SetColor(const Ogre::ColourValue& val);

		// ResizableImage::Listener
		void OnChanged();
		void OnDestroy();


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
		ResizableImage* m_Image;

		Ogre::ColourValue m_Color;

		void GetNumTiles(int& numTiles, int& tilesHorizontal, int& tilesVertical);
		void AddTile(float row, float col, TileType tileType, float width, float height, Vertex2DTex* vertBuffer, int& vertexOffset);
		void GetTextureCoordinates(TileType type, Ogre::FloatRect& rect);
	};
}

#endif // __WmeResizableElement2D_H__