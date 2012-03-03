// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeScene2DBase_H__
#define __WmeScene2DBase_H__


#include "Scene.h"


namespace Wme
{
	class Viewport;
	class Canvas2D;

	class WmeDllExport Scene2DBase : public Scene
	{
	public:
		Scene2DBase();
		virtual ~Scene2DBase();

		virtual void Create();
		virtual void Update();
		virtual void Shutdown();

		Viewport* GetViewport() const { return m_Viewport; }
		void SetViewport(Viewport* viewport);


	protected:
		Canvas2D* m_Canvas;
		Viewport* m_Viewport;
	};
}

#endif // __WmeScene2DBase_H__
