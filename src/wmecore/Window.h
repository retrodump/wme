// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeWindow_H__
#define __WmeWindow_H__


#include "UiObjectBase.h"


namespace Wme
{
	class ResizableElement2D;
	// temp
	class ResizableImage;

	class WmeDllExport Window : public UiObjectBase
	{
	public:
		Window(Canvas2D* canvas);
		virtual ~Window();


		// ScriptableObject
		RTTI(Window);
		virtual WideString ConvertToString() { return L"Window object"; }
		virtual WideString GetTypeName() const { return L"Window"; }


	private:
		SceneNode2D* m_BackgroundNode;
		ResizableElement2D* m_Background;
		ResizableImage* m_ResizableTexture;

		virtual void OnSizeChanged();
	};
}

#endif // __WmeWindow_H__