// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeSpriteFrame_H__
#define __WmeSpriteFrame_H__

#include "ScriptableObject.h"
#include "Sprite.h"

namespace Wme
{
	class SpriteSubFrame;
	class ElementCollection;

	class WmeDllExport SpriteFrame : public ScriptableObject
	{
		friend class Sprite;
	public:		
		SpriteFrame();
		virtual ~SpriteFrame();


		void Display(ElementCollection* elementCol, int targetX, int targetY, const SpriteDrawingParams& params) const;

		void Interpolate(SpriteFrame* nextFrame, float lerpValue);

		typedef std::vector<SpriteSubFrame*> SubFrameCollection;
		SubFrameCollection& GetSubFrames() { return m_SubFrames; }

		unsigned long GetDelay() { return m_Delay; }
		void SetDelay(unsigned long delay) { m_Delay = delay; }

		void GetBoundingRect(Rect& rect) const;
		
        // DocumentAwareObject
		virtual bool LoadFromXml(TiXmlElement* rootNode);
		virtual bool SaveToXml(TiXmlElement* rootNode);		

        // ScriptableObject
		RTTI(SpriteFrame);
		virtual WideString ConvertToString() { return L"SpriteFrame object"; }
		virtual WideString GetTypeName() const { return L"SpriteFrame"; }

	private:
		bool LoadFromImage(const WideString& fileName);
		SubFrameCollection m_SubFrames;
		unsigned long m_Delay;
	};
}


#endif