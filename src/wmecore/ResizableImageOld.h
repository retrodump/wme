// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeResizableImageOld_H__
#define __WmeResizableImageOld_H__


#include "Object.h"
#include "IRenderableProvider.h"
#include "DocumentAwareObject.h"


namespace Wme
{
	class SpriteTexture;
	class Viewport;
	class ElementCollection;
	class SpriteDrawingParams;


	//////////////////////////////////////////////////////////////////////////
	class WmeDllExport ImageTile
	{
	public:
		enum TileType
		{
			TILE_ALL,
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

		ImageTile(int x, int y, int width, int height, TileType type)
		{
			m_Rect = Ogre::FloatRect((float)x, (float)y, (float)(x + width), (float)(y + height));
			m_Type = type;
		}

		TileType m_Type;
		Ogre::FloatRect m_Rect;

	};

	//////////////////////////////////////////////////////////////////////////
	class WmeDllExport ResizableImageOld : public Object, public DocumentAwareObject, public Ogre::Renderable, public IRenderableProvider
	{
	public:
		ResizableImageOld();
		~ResizableImageOld();

		enum ImageType
		{
			IMAGE_TILED,
			IMAGE_TILED_FRAME,
			IMAGE_STRETCHED
		};

		void Display(ElementCollection* elementCol, const SpriteDrawingParams& params);

		void SetPos(int posX, int posY);
		void SetSize(int width, int height);

		int GetWidth() const { return m_Width; }
		int GetHeight() const { return m_Height; }

		int GetCorrectedWidth() const;
		int GetCorrectedHeight() const;

		ImageType GetType() const { return m_Type; }
		void SetType(ImageType type) { m_Type = type; m_IsDirty = true; }

		const SpriteTexture* GetTexture() const { return m_Texture; }
		bool SetTexture(const WideString& fileName, Ogre::ColourValue colorKey = Ogre::ColourValue::ZERO);

		Ogre::ColourValue GetColor() const { return m_Color; }
		void SetColor(const Ogre::ColourValue& color)
		{
			if (color != m_Color) m_IsDirty = true;
			m_Color = color;
		}

		size_t GetFrameTopHeight() const { return m_FrameTopHeight; }
		void SetFrameTopHeight(size_t val) { m_FrameTopHeight = val; m_IsDirty = true; }

		size_t GetFrameBottomHeight() const { return m_FrameBottomHeight; }
		void SetFrameBottomHeight(size_t val) { m_FrameBottomHeight = val; m_IsDirty = true; }

		size_t GetFrameLeftWidth() const { return m_FrameLeftWidth; }
		void SetFrameLeftWidth(size_t val) { m_FrameLeftWidth = val; m_IsDirty = true; }

		size_t GetFrameRightWidth() const { return m_FrameRightWidth; }
		void SetFrameRightWidth(size_t val) { m_FrameRightWidth = val; m_IsDirty = true; }

		void UpdateGeometry(Viewport* viewport, bool absolutePos);

		// DocumentAwareObject
		bool LoadFromXml(TiXmlElement* rootNode);
		bool SaveToXml(TiXmlElement* rootNode);
		Utf8String GetDocRootName() { return "WmeResizableImage"; }


		// IRenderableProvider implementation
		virtual void GetRenderables(IRenderableProvider::RenderableList& renderableList);

		// Ogre::Renderable implementation
		virtual const Ogre::MaterialPtr& getMaterial(void) const;
		virtual void getRenderOperation(Ogre::RenderOperation& op);
		virtual void getWorldTransforms(Ogre::Matrix4* xform) const;
		Ogre::Real getSquaredViewDepth(const Ogre::Camera* cam) const { return 0; }
		const Ogre::LightList& getLights(void) const;

	private:
		void BuildTiles();
		void FillTiles(Viewport* viewport, bool absolutePos);
		void GetTextureCoordinates(ImageTile::TileType type, Ogre::FloatRect& rect);
		void GetCorrectedSize(int& middleWidth, int& middleHeight, int& middleTileWidth, int& middleTileHeight) const;

		bool m_IsDirty;
		int m_UpdateId;

		ImageType m_Type;
		SpriteTexture* m_Texture;
		Ogre::ColourValue m_Color;

		size_t m_MaxTiles;

		std::list<ImageTile> m_Tiles;

		int m_PosX;
		int m_PosY;
		int m_Width;
		int m_Height;

		mutable int m_CorrectedWidth;
		mutable int m_CorrectedHeight;

		size_t m_FrameTopHeight;
		size_t m_FrameBottomHeight;
		size_t m_FrameLeftWidth;
		size_t m_FrameRightWidth;

		Ogre::HardwareVertexBufferSharedPtr m_HardwareBuffer;
		Ogre::RenderOperation m_RenderOperation;
		bool m_HWBufferInitialized;
	};
}

#endif // __WmeResizableImageOld_H__
