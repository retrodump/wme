// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeFullScene_H__
#define __WmeFullScene_H__

#include "Scene3DBase.h"
#include "MouseEvent.h"
#include "KeyboardEvent.h"


namespace Wme
{
	class Viewport;
	class Camera;
	class Actor3D; // temp

	class WmeDllExport FullScene : public Scene3DBase
	{
	public:
		FullScene();
		virtual ~FullScene();

		virtual void Create();
		virtual void Update();

		virtual void Shutdown();

		virtual bool HandleMouseEvent(Viewport* viewport, MouseEvent& event);
		virtual bool HandleKeyboardEvent(Viewport* viewport, KeyboardEvent& event);

		// ScriptableObject
		RTTI(FullScene);
		virtual WideString ConvertToString() { return L"Full scene object"; }
		virtual WideString GetTypeName() const { return L"FullScene"; }


	private:
		Actor3D* m_Actor;
		Actor3D* m_Actor2;
	};
}


#endif // __WmeFullScene_H__