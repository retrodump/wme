// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "ViewportLayout.h"
#include "View.h"
#include "Viewport.h"
#include "Camera.h"
#include "StringUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
ViewportLayout::ViewportLayout(View* parentView)
{
	m_ParentView = parentView;
}

//////////////////////////////////////////////////////////////////////////
ViewportLayout::~ViewportLayout()
{
	foreach (Viewport* viewport, m_Viewports)
	{
		SAFE_DELETE(viewport);
	}
}

//////////////////////////////////////////////////////////////////////////
void ViewportLayout::Update()
{
	foreach (Viewport* viewport, m_Viewports)
	{
		viewport->Update();
	}
}

//////////////////////////////////////////////////////////////////////////
Viewport* ViewportLayout::AddViewport(const WideString& name, Camera* camera, float left, float top, float width, float height)
{
	Viewport* viewport = new Viewport();
	viewport->Register();
	viewport->Create(this, camera, left, top, width, height);
	viewport->SetName(name);

	m_Viewports.push_back(viewport);

	return viewport;
}

//////////////////////////////////////////////////////////////////////////
Viewport* ViewportLayout::InsertViewport(Viewport* insertPoint, bool insertAfter, const WideString& name, Camera* camera, float left, float top, float width, float height)
{
	Viewport* viewport = new Viewport();
	viewport->Create(this, camera, left, top, width, height);
	viewport->SetName(name);

	ViewportList::iterator it;

	for (it = m_Viewports.begin(); it != m_Viewports.end(); it++)
	{
		if ((*it) == insertPoint)
		{
			if (insertAfter) it++;
			m_Viewports.insert(it, viewport);
			return viewport;
		}
	}
	m_Viewports.push_back(viewport);

	return viewport;
}

//////////////////////////////////////////////////////////////////////////
void ViewportLayout::RemoveViewport(Viewport* viewport)
{
	foreach (Viewport* vp, m_Viewports)
	{
		if (vp == viewport)
		{
			m_Viewports.remove(viewport);
			SAFE_DELETE(viewport);
			return;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void ViewportLayout::OnCameraDestroyed(Camera* camera)
{
	foreach (Viewport* viewport, m_Viewports)
	{
		if (viewport->GetCamera() == camera) viewport->SetCamera(NULL);
	}
}

//////////////////////////////////////////////////////////////////////////
void ViewportLayout::ReleaseOgreViewports()
{
	foreach (Viewport* viewport, m_Viewports)
	{
		viewport->ReleaseOgreViewports();
	}
}

//////////////////////////////////////////////////////////////////////////
void ViewportLayout::RebuildViewportStack()
{
	m_ParentView->RemoveAllOgreViewports();

	int zOrder = 0;
	foreach (Viewport* viewport, m_Viewports)
	{
		viewport->ReleaseOgreViewports();
		viewport->RebuildOgreViewports(zOrder);
	}
}

//////////////////////////////////////////////////////////////////////////
void ViewportLayout::InvalidateViewports()
{
	foreach (Viewport* viewport, m_Viewports)
	{
		viewport->SetDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void ViewportLayout::OnResizeViewports()
{
	foreach (Viewport* viewport, m_Viewports)
	{
		viewport->OnResize();
	}
}

//////////////////////////////////////////////////////////////////////////
Viewport* ViewportLayout::GetViewportByName(const WideString& name)
{
	foreach (Viewport* viewport, m_Viewports)
	{
		if (StringUtil::CompareNoCase(viewport->GetName(), name)) return viewport;
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
bool ViewportLayout::GetObjectAt(int x, int y, MousePickResult& result)
{
	reverse_foreach (Viewport* viewport, m_Viewports)
	{		
		// is the point within this viewport?
		if (viewport->IsPointInViewport(x, y))
		{
			Ogre::Viewport* vp = viewport->GetOgreViewport();

			if (viewport->GetObjectAt(x - vp->getActualLeft(), y - vp->getActualTop(), result)) return true;

			if (!viewport->IsClickThrough()) return NULL;
		}		

	}
	return false;
}


//////////////////////////////////////////////////////////////////////////
bool ViewportLayout::HandleMouseEvent(MouseEvent& event)
{
	reverse_foreach (Viewport* viewport, m_Viewports)
	{		
		if (viewport->IsPointInViewport(event.GetPosX(), event.GetPosY()))
		{
			Viewport* eventTarget = viewport;
			MouseEvent eventParams = event;

			// redirect to RTT viewport if necessary
			MousePickResult pickResult;
			if (GetObjectAt(event.GetPosX(), event.GetPosY(), pickResult))
			{
				if (pickResult.Viewport && pickResult.Viewport != eventTarget)
				{
					eventTarget = pickResult.Viewport;

					int x, y;
					eventTarget->GetParentLayout()->GetParentView()->GetMousePos(x, y);
					eventParams.SetPos(x, y);
				}
			}

			if (eventTarget->HandleMouseEvent(eventTarget, eventParams)) return true;
			else if (!viewport->IsClickThrough()) return false;
		}		
	}

	return false;
}


//////////////////////////////////////////////////////////////////////////
bool ViewportLayout::HandleKeyboardEvent(KeyboardEvent& event)
{
	reverse_foreach (Viewport* viewport, m_Viewports)
	{		
		if (viewport->HandleKeyboardEvent(event)) return true;
	}

	return false;
}


} // namespace Wme
