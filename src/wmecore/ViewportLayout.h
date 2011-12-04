// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeViewportLayout_H__
#define __WmeViewportLayout_H__


#include "Object.h"
#include "MouseEvent.h"
#include "KeyboardEvent.h"
#include "MousePickParams.h"


namespace Wme
{
	class View;
	class Viewport;
	class Camera;
	class ScriptableObject;

	class WmeDllExport ViewportLayout : public Object
	{
	public:
		ViewportLayout(View* parentView);
		~ViewportLayout();

		void Update();

		Viewport* AddViewport(const WideString& name, Camera* camera, float left = 0.0f, float top = 0.0f, float width = 1.0f, float height = 1.0f);
		Viewport* InsertViewport(Viewport* insertPoint, bool insertAfter, const WideString& name, Camera* camera, float left, float top, float width, float height);
		void RemoveViewport(Viewport* viewport);

		void ReleaseOgreViewports();
		void RebuildViewportStack();

		void OnCameraDestroyed(Camera* camera);

		Viewport* GetViewportByName(const WideString& name);

		View* GetParentView() { return m_ParentView; }

		bool GetObjectAt(int x, int y, MousePickResult& result);

		void InvalidateViewports();
		void OnResizeViewports();

		// events
		bool HandleMouseEvent(MouseEvent& event);
		bool HandleKeyboardEvent(KeyboardEvent& event);


	private:
		typedef std::list<Viewport*> ViewportList;
		ViewportList m_Viewports;
		View* m_ParentView;
	};
}

#endif // __WmeViewportLayout_H__
