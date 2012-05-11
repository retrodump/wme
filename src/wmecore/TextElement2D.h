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

	public:
		TextElement2D();
		virtual ~TextElement2D();

		void AddGeometry();

		void SetFont(Font* font);
		Font* GetFont() const { return m_Font; }

		void SetText(const WideString& text);
		const WideString& GetText() const { return m_Text; }

		void SetColor(const Ogre::ColourValue& color);
		Ogre::ColourValue GetColor() const { return m_Color; }

		void SetAlignment(const Alignment& alignment);
		Alignment GetAlignment() const { return m_Alignment; }

		void SetWidth(int width);
		int GetWidth() const { return m_Width; }

		void SetHeight(int height);
		int GetHeight() const { return m_Height; }

		void SetLeadingSpace(int leadingSpace);
		int GetLeadingSpace() const { return m_LeadingSpace; }

	private:
		Font* m_Font;
		WideString m_Text;
		Ogre::ColourValue m_Color;
		Alignment m_Alignment;
		int m_Width;
		int m_Height;
		int m_LeadingSpace;

		typedef std::map<int, RenderBatch*> RenderBatchMap;

		void InitializeRenderBatches(RenderBatchMap& renderBatches);
		void GenerateRenderBatches(RenderBatchMap& renderBatches);
		void AddCharacter(wchar_t ch, int x, int y, GlyphInfo* glyphInfo, RenderBatchMap& renderBatches);
	};
}

#endif // __WmeTextElement2D_H__