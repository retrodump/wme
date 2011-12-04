// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeHybridScene_H__
#define __WmeHybridScene_H__


#include "Scene3DBase.h"


namespace Wme
{
	class ElementCollection;
	class SpriteSubFrame;
	class Viewport;

	class WmeDllExport HybridScene : public Scene3DBase
	{
	public:

		HybridScene();
		~HybridScene();

		void Create();
		virtual void Render(Viewport* viewport, Camera* camera);

        // ScriptableObject
		RTTI(HybridScene);
		virtual WideString ConvertToString() { return L"Hybrid scene object";	}
		virtual WideString GetTypeName() const { return L"HybridScene"; }

	private:
		ElementCollection* m_Elements;
		SpriteSubFrame* m_Background;
	};
}

#endif // __WmeHybridScene_H__
