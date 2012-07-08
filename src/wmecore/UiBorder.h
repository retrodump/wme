// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeUiBorder_H__
#define __WmeUiBorder_H__


#include "UiElement.h"
#include "StateVar.h"


namespace Wme
{
	class ResizableImage;
	class ResizableElement2D;

	class WmeDllExport UiBorder : public UiElement
	{
	public:
		UiBorder(Canvas2D* canvas);
		virtual ~UiBorder();

		// ScriptableObject
		RTTI(UiBorder);
		virtual WideString ConvertToString() { return L"Border object"; }
		virtual WideString GetTypeName() const { return L"Border"; }


		ResizableImage* GetImage(const WideString& stateName = L"") const { return m_Image.GetValue(stateName); }
		void SetImage(ResizableImage* image, const WideString& stateName = L"");
		void SetImage(const WideString& fileName, const WideString& stateName = L"");

		virtual void OnSizeChanged();
		virtual void OnStateChanged();

	private:
		StateVar<ResizableImage*> m_Image;
		ResizableElement2D* m_Elem;
	};
}

#endif // __WmeUiBorder_H__
