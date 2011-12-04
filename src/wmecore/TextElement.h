// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeTextElement_H__
#define __WmeTextElement_H__

#include "Object.h"
#include "OgreRenderable.h"
#include "IRenderableProvider.h"
#include "Alignment.h"
#include "Rect.h"

namespace Wme
{
	class Font;
	class GlyphInfo;
	class Viewport;
	class ElementCollection;
	class SpriteDrawingParams;

	class WmeDllExport TextElement : public Object, public IRenderableProvider
	{
	public:
		TextElement();
		virtual ~TextElement();

		void Display(ElementCollection* elementCol, const SpriteDrawingParams& params);
		void UpdateGeometry(Viewport* viewport, bool absolutePos);

		Ogre::RenderOperation& GetRenderOp() { return m_RenderOperation; }

		void SetFont(Font* font);
		Font* GetFont() const { return m_Font; }


		void SetText(const WideString& text)
		{
			if (m_Text != text)
			{
				m_IsDirty = true;
				m_Text = text;
			}
		}
		const WideString& GetText() const { return m_Text; }


		void SetMaxWidth(int maxWidth)
		{
			if (m_MaxWidth != maxWidth)
			{
				m_IsDirty = true;
				m_MaxWidth = maxWidth;
			}
		}
		int GetMaxWidth() const { return m_MaxWidth; }


		void SetMaxHeight(int maxHeight)
		{
			if (m_MaxHeight != maxHeight)
			{
				m_IsDirty = true;
				m_MaxHeight = maxHeight;
			}
		}
		int GetMaxHeight() const { return m_MaxHeight; }


		void SetPosX(int posX)
		{
			if (m_PosX != posX)
			{
				m_IsDirty = true;
				m_PosX = posX;
			}
		}
		int GetPosX() const { return m_PosX; }


		void SetPosY(int posY)
		{
			if (m_PosY != posY)
			{
				m_IsDirty = true;
				m_PosY = posY;
			}
		}
		int GetPosY() const { return m_PosY; }

		void SetRect(const Rect& rect);

		void SetAlignment(const Alignment& alignment);
		Alignment GetAlignment() const { return m_Alignment; }

		void SetColor(const Ogre::ColourValue& color)
		{
			if (m_Color != color)
			{
				m_IsDirty = true;
				m_Color = color;
			}
		}
		Ogre::ColourValue GetColor() const { return m_Color; }


		// IRenderableProvider implementation
		virtual void GetRenderables(IRenderableProvider::RenderableList& renderableList);

	private:
		//////////////////////////////////////////////////////////////////////////
		class WmeDllExport TextRenderBatch : public Ogre::Renderable
		{
		public:
			TextRenderBatch()
			{
				m_BatchIndex = 0;
				m_StartIndex = m_CurrentIndex = 0;
				m_NumChars = 0;
				m_TextElement = NULL;

				mUseIdentityProjection = true;
				mUseIdentityView = true;
				mPolygonModeOverrideable = false;
			}

			int m_BatchIndex;
			int m_StartIndex;
			int m_NumChars;
			int m_CurrentIndex;
			TextElement* m_TextElement;

			// Ogre::Renderable
			virtual const Ogre::MaterialPtr& getMaterial(void) const;
			virtual void getRenderOperation(Ogre::RenderOperation& op);
			virtual void getWorldTransforms(Ogre::Matrix4* xform) const;
			Ogre::Real getSquaredViewDepth(const Ogre::Camera* cam) const { return 0; }
			const Ogre::LightList& getLights(void) const;
		};


		void ClearRenderBatches();
		void UpdateRenderBatches();

		void AddCharacter(wchar_t ch, int X, int Y, GlyphInfo* glyphInfo, SpriteVertex* vertexBuf, Viewport* viewport, bool absolutePos);

		void ResizeHWBuffer(size_t numCharacters);
		void DestroyHWBuffer();
		Ogre::HardwareVertexBufferSharedPtr m_HardwareBuffer;
		Ogre::RenderOperation m_RenderOperation;

		bool m_HWBufferInitialized;
		size_t m_NumCharacters;

		bool m_IsDirty;
		int m_UpdateId;

		WideString m_Text;
		Ogre::ColourValue m_Color;

		Font* m_Font;

		int m_PosX;
		int m_PosY;

		int m_MaxWidth;
		int m_MaxHeight;

		Alignment m_Alignment;

		typedef std::vector<TextRenderBatch*> RenderBatches;
		RenderBatches m_RenderBatches;

	};
}

#endif // __WmeTextElement_H__
