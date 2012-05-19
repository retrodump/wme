// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeTextElement2D_H__
#define __WmeTextElement2D_H__


#include "Element2D.h"
#include "Alignment.h"


namespace Wme
{
	class Font;
	class GlyphInfo;

	class WmeDllExport TextElement2D : public Element2D
	{
	private:
		//////////////////////////////////////////////////////////////////////////
		class RenderBatch
		{
		public:
			RenderBatch();
			~RenderBatch();
			
			void IncrementCapacity();
			void AllocateVertices();
			void AddCharacter(const Ogre::Vector2& topLeft, const Ogre::Vector2& bottomRight, const Ogre::FloatRect& texCoords, const Ogre::ColourValue& color);

			Vertex2DTex* GetVertices() const { return m_Vertices; }
			size_t GetNumCharacters() const { return m_NumCharacters; }

		private:
			size_t m_NumCharacters;
			size_t m_Capacity;
			Vertex2DTex* m_Vertices;
		};

		//////////////////////////////////////////////////////////////////////////
		class Stroke
		{
		public:
			Stroke(float startX, float endX, float posY, float thickness)
			{
				m_StartX = startX;
				m_EndX = endX;
				m_PosY = posY;
				m_Thickness = thickness;
			}

			float GetStartX() const { return m_StartX; }
			float GetEndX() const { return m_EndX; }
			float GetPosY() const { return m_PosY; }
			float GetThickness() const { return m_Thickness; }

		private:
			float m_StartX;
			float m_EndX;
			float m_PosY;
			float m_Thickness;
		};


	public:
		TextElement2D();
		virtual ~TextElement2D();

		enum DecorationType
		{
			DECORATION_NONE,
			DECORATION_SHADOW,
			DECORATION_OUTLINE
		};

		void AddGeometry();

		void SetFont(Font* font);
		Font* GetFont() const { return m_Font; }

		void SetText(const WideString& text);
		const WideString& GetText() const { return m_Text; }

		void SetColor(const Ogre::ColourValue& color);
		Ogre::ColourValue GetColor() const { return m_Color; }

		void SetDecorationColor(const Ogre::ColourValue& color);
		Ogre::ColourValue GetDecorationColor() const { return m_DecorationColor; }

		void SetAlignment(const Alignment& alignment);
		Alignment GetAlignment() const { return m_Alignment; }

		void SetWidth(int width);
		int GetWidth() const { return m_Width; }

		void SetHeight(int height);
		int GetHeight() const { return m_Height; }

		void SetLeadingSpace(int leadingSpace);
		int GetLeadingSpace() const { return m_LeadingSpace; }

		void SetDecorationType(DecorationType type);
		DecorationType GetDecorationType() const { return m_DecorationType; }

		void SetDecorationThickness(float thickness);
		float GetDecorationThickness() const { return m_DecorationThickness; }


	private:
		struct Layer
		{
			Layer(float offsetX, float offsetY, bool decoration)
			{
				OffsetX = offsetX;
				OffsetY = offsetY;
				Decoration = decoration;
			}
			float OffsetX;
			float OffsetY;
			bool Decoration;
		};

		Font* m_Font;
		WideString m_Text;
		Ogre::ColourValue m_Color;
		Ogre::ColourValue m_DecorationColor;
		Alignment m_Alignment;
		int m_Width;
		int m_Height;
		int m_LeadingSpace;
		Ogre::MaterialPtr m_StrokeMaterial;
		DecorationType m_DecorationType;
		float m_DecorationThickness;		

		typedef std::map<int, RenderBatch*> RenderBatchMap;
		typedef std::list<Stroke*> StrokeList;
		typedef std::list<Layer> LayerList;

		void InitializeRenderBatches(RenderBatchMap& renderBatches);
		void GenerateRenderBatches(RenderBatchMap& renderBatches, StrokeList& strokes, const Layer& layer);
		void AddCharacter(wchar_t ch, int x, int y, GlyphInfo* glyphInfo, RenderBatchMap& renderBatches, const Layer& layer);
		void RenderStrokes(StrokeList& strokes, const Layer& layer);
		void RenderStroke(const Stroke* stroke, Vertex2D* vertBuffer, int& vertexOffset, const Layer& layer);
		void GenerateLayers(LayerList& layers);
	};
}

#endif // __WmeTextElement2D_H__