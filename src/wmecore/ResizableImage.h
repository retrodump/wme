// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeResizableImage_H__
#define __WmeResizableImage_H__


#include "Object.h"
#include "DocumentAwareObject.h"


namespace Wme
{
	class SpriteTexture;

	class WmeDllExport ResizableImage : public Object, public DocumentAwareObject
	{
	public:

		class WmeDllExport Listener 
		{
		public:
			Listener() {}
			virtual ~Listener() {}

			virtual void OnChanged() {}
			virtual void OnDestroy() {}
		};


		ResizableImage();
		virtual ~ResizableImage();

		SpriteTexture* GetTexture() const { return m_Texture; }
		bool SetTexture(const WideString& fileName, Ogre::ColourValue colorKey = Ogre::ColourValue::ZERO);

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

		void AddListener(Listener* listener);
		void RemoveListener(Listener* listener);

		// DocumentAwareObject
		bool LoadFromXml(TiXmlElement* rootNode);
		bool SaveToXml(TiXmlElement* rootNode);
		Utf8String GetDocRootName() { return "WmeResizableImage"; }

	private:
		SpriteTexture* m_Texture;

		float m_FrameTopHeight;
		float m_FrameBottomHeight;
		float m_FrameLeftWidth;
		float m_FrameRightWidth;

		bool m_StretchHorizontal;
		bool m_StretchVertical;
		bool m_PaintMiddlePart;

		typedef std::vector<Listener*> ListenerList;
		ListenerList m_Listeners;

		void InvokeChanged() const;
	};
}

#endif // __WmeResizableImage_H__
