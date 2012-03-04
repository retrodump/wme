// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Viewport.h"
#include "ContentManager.h"
#include "View.h"
#include "Camera.h"
#include "CameraController.h"
#include "Stage.h"
#include "ViewportLayout.h"
#include "ActiveSpot.h"
#include "StageEditor.h"
#include "MathUtil.h"


namespace Wme
{

//http://www.ogre3d.org/wiki/index.php/GetScreenspaceCoords

//////////////////////////////////////////////////////////////////////////
Viewport::Viewport()
{
	m_ParentLayout = NULL;
	m_Camera = NULL;
	m_CameraController = NULL;
	m_OgreViewport = NULL;
	
	m_DesignWidth = 0;
	m_DesignHeight = 0;
	m_VirtualWidth = 0;
	m_VirtualHeight = 0;

	m_RelativeLeft = m_RelativeTop = m_RelativeWidth = m_RelativeHeight = 0;

	m_ScrollOffsetX = m_ScrollOffsetY = 0;

	m_ScaleFactor = 1.0f;
	m_AutoScale = false;

	m_IsClickThrough = true;

	m_ClearBeforePaint = true;

	m_IsRendering = false;
	m_UpdateId = 0;
}

//////////////////////////////////////////////////////////////////////////
Viewport::~Viewport()
{
	SAFE_DELETE(m_CameraController);

	if (HasMouseCapture())
	{
		Game::GetInstance()->GetContentMgr()->ReleaseMouse(this);
	}
	ClearActiveSpots();
}


//////////////////////////////////////////////////////////////////////////
void Viewport::Create(ViewportLayout* parentLayout, Camera* camera, float left, float top, float width, float height)
{
	SetDirty();
	m_Camera = camera;
	
	m_RelativeLeft = left;
	m_RelativeTop = top;
	m_RelativeWidth = width;
	m_RelativeHeight = height;

	m_ParentLayout = parentLayout;
}

//////////////////////////////////////////////////////////////////////////
void Viewport::Update()
{
	if (m_CameraController) m_CameraController->Update();
}

//////////////////////////////////////////////////////////////////////////
View* Viewport::GetParentView() const
{
	if (m_ParentLayout) return m_ParentLayout->GetParentView();
	else return NULL;
}

//////////////////////////////////////////////////////////////////////////
void Viewport::SetCamera(Camera* camera)
{
	m_Camera = camera;
	if (m_OgreViewport)
	{
		if (camera) m_OgreViewport->setCamera(camera->GetOgreCamera());
		else m_OgreViewport->setCamera(NULL);
	}
}

//////////////////////////////////////////////////////////////////////////
void Viewport::SetCameraController(CameraController* controller)
{
	if (controller == m_CameraController) return;

	SAFE_DELETE(m_CameraController);
	m_CameraController = controller;
}

//////////////////////////////////////////////////////////////////////////
void Viewport::SetClearBeforePaint(bool val)
{
	m_ClearBeforePaint = val;
	if (m_OgreViewport) m_OgreViewport->setClearEveryFrame(val);
}

//////////////////////////////////////////////////////////////////////////
void Viewport::TransformVertices(SpriteVertex* vertices, size_t numVertices, bool absolutePos)
{
	// OBSOLETE

	Ogre::RenderSystem* rs = Ogre::Root::getSingleton().getRenderSystem();


	// scroll offset
	if (!absolutePos)
	{
		for (size_t i = 0; i < numVertices; i++)
		{
			vertices[i].pos.x -= m_ScrollOffsetX;
			vertices[i].pos.y -= m_ScrollOffsetY;
		}
	}

	// deal with resolution changes
	Ogre::Real widthRatio, heightRatio;
	widthRatio = GetScaleX();
	heightRatio = GetScaleY();


	// correction for different texel origin in D3D/OGL
	Ogre::Real hOffset = rs->getHorizontalTexelOffset() / (0.5f * m_OgreViewport->getActualWidth());
	Ogre::Real vOffset = rs->getVerticalTexelOffset() / (0.5f * m_OgreViewport->getActualHeight());

	float halfHeight = m_OgreViewport->getHeight() / 2.0f;

	// final position in screen-relative, resolution-relative coords
	for (size_t i = 0; i < numVertices; i++)
	{
		vertices[i].pos.x = vertices[i].pos.x / (float)m_OgreViewport->getActualWidth()  * widthRatio;
		vertices[i].pos.y = vertices[i].pos.y / (float)m_OgreViewport->getActualHeight() * heightRatio;

		// convert to Ogre screen coordinates
		vertices[i].pos.x = vertices[i].pos.x * 2 - 1          + hOffset;
		vertices[i].pos.y = 1 - vertices[i].pos.y / halfHeight - vOffset;
	}
}

//////////////////////////////////////////////////////////////////////////
void Viewport::Transform2DVertices(byte* vertexData, size_t numVertices, size_t vertexSize)
{
	// transforms pretransformed screen-space 2D vertices to Ogre viewport-space

	Ogre::RenderSystem* rs = Ogre::Root::getSingleton().getRenderSystem();

	// deal with resolution changes
	Ogre::Real widthRatio, heightRatio;
	widthRatio = GetScaleX();
	heightRatio = GetScaleY();


	// correction for different texel origin in D3D/OGL
	Ogre::Real hOffset = rs->getHorizontalTexelOffset() / (0.5f * m_OgreViewport->getActualWidth());
	Ogre::Real vOffset = rs->getVerticalTexelOffset() / (0.5f * m_OgreViewport->getActualHeight());

	float halfHeight = m_OgreViewport->getHeight() / 2.0f;


	// process vertices
	for (size_t i = 0; i < numVertices; i++)
	{
		Ogre::Vector2* vertices = reinterpret_cast<Ogre::Vector2*>(vertexData);

		vertices->x = vertices->x / (float)m_OgreViewport->getActualWidth()  * widthRatio;
		vertices->y = vertices->y / (float)m_OgreViewport->getActualHeight() * heightRatio;

		// convert to Ogre screen coordinates
		vertices->x = vertices->x * 2 - 1          + hOffset;
		vertices->y = 1 - vertices->y / halfHeight - vOffset;

		vertexData += vertexSize;
	}
}

//////////////////////////////////////////////////////////////////////////
void Viewport::RotateVertices(SpriteVertex* verts, size_t numVerts, const Ogre::Vector2& centerPoint, float angle)
{
	Ogre::Matrix4 matTrans1 = Ogre::Matrix4::IDENTITY;
	matTrans1.makeTrans(-centerPoint.x, -centerPoint.y, 0.0);

	Ogre::Matrix4 matTrans2;
	matTrans2.makeTrans(centerPoint.x, centerPoint.y, 0.0);

	Ogre::Matrix4 matRotation(Ogre::Quaternion(Ogre::Radian(Ogre::Degree(angle)), Ogre::Vector3::UNIT_Z));

	Ogre::Matrix4 matFinal = matTrans2 * matRotation * matTrans1;

	for (size_t i = 0; i < numVerts; i++)
	{
		verts[i].pos = matFinal * verts[i].pos;
	}
}

//////////////////////////////////////////////////////////////////////////
void Viewport::TransformClippingRect(Rect& rect, bool absolutePos)
{
	if (!absolutePos)
	{
		rect.x -= m_ScrollOffsetX;
		rect.y -= m_ScrollOffsetY;
	}

	Ogre::Real widthRatio, heightRatio;
	widthRatio = GetScaleX();
	heightRatio = GetScaleY();

	rect.x = rect.x * widthRatio;
	rect.width = rect.width * widthRatio;

	rect.y = rect.y * heightRatio;
	rect.height = rect.height * heightRatio;

	rect.x += m_OgreViewport->getActualLeft();
	rect.y += m_OgreViewport->getActualTop();
}

//////////////////////////////////////////////////////////////////////////
void Viewport::SetDimensions(size_t designWidth, size_t designHeight, size_t virtualWidth, size_t virtualHeight)
{
	SetDirty();

	m_DesignWidth = designWidth;
	m_DesignHeight = designHeight;
	m_VirtualWidth = virtualWidth;
	m_VirtualHeight = virtualHeight;

	if (m_VirtualWidth == 0) m_VirtualWidth = m_DesignWidth;
	if (m_VirtualHeight == 0) m_VirtualHeight = m_DesignHeight;
}

//////////////////////////////////////////////////////////////////////////
void Viewport::SetScrollOffsetX(int newOffset)
{
	SetDirty();
	m_ScrollOffsetX = newOffset;
}

//////////////////////////////////////////////////////////////////////////
void Viewport::SetScrollOffsetY(int newOffset)
{
	SetDirty();
	m_ScrollOffsetY = newOffset;
}

//////////////////////////////////////////////////////////////////////////
float Viewport::GetScaleX() const
{
	if (m_AutoScale)
	{
		return (float)m_OgreViewport->getActualWidth() / (float)m_DesignWidth;
	}
	else
	{
		return m_ScaleFactor;
	}
}

//////////////////////////////////////////////////////////////////////////
float Viewport::GetScaleY() const
{
	if (m_AutoScale)
	{
		return (float)m_OgreViewport->getActualHeight() / (float)m_DesignHeight;
	}
	else
	{
		return m_ScaleFactor;
	}
}

//////////////////////////////////////////////////////////////////////////
void Viewport::PreDisplay()
{
	m_IsRendering = true;
	ClearActiveSpots();

	m_ClippingRectStack.clear();
	//m_ClippingRect = Rect(0, 0, (float)m_DesignWidth, (float)m_DesignHeight);
	m_ClippingRect = Rect(0, 0, (float)m_OgreViewport->getActualWidth() / GetScaleX(), (float)m_OgreViewport->getActualHeight() / GetScaleY());


	if (m_Camera)
	{
		m_Camera->GetStage()->Render(this, m_Camera);
	}
}

//////////////////////////////////////////////////////////////////////////
void Viewport::PostDisplay()
{
	m_IsRendering = false;
}

//////////////////////////////////////////////////////////////////////////
void Viewport::OnResize()
{
	if (m_Camera && !m_Camera->HasCustomAspectRatio())
	{
		m_Camera->GetOgreCamera()->setAspectRatio((Ogre::Real)m_OgreViewport->getActualWidth() / (Ogre::Real)m_OgreViewport->getActualHeight());
	}
}

//////////////////////////////////////////////////////////////////////////
void Viewport::ReleaseOgreViewports()
{
	m_OgreViewport = NULL;
	if (m_Camera && m_Camera->GetStage()) m_Camera->GetStage()->ReleaseOgreViewports(this);
}

//////////////////////////////////////////////////////////////////////////
void Viewport::RebuildOgreViewports(int& zOrder)
{
	m_OgreViewport = m_ParentLayout->GetParentView()->GetRenderTarget()->addViewport(m_Camera->GetOgreCamera(), ++zOrder, m_RelativeLeft, m_RelativeTop, m_RelativeWidth, m_RelativeHeight);
	m_ParentLayout->GetParentView()->AddViewportToMap(this, m_OgreViewport);
	
	if (m_Camera && m_Camera->GetStage())
	{
		m_Camera->GetStage()->OnViewportCreated(this);
		m_Camera->GetStage()->RebuildOgreViewports(zOrder, this);
	}
	m_OgreViewport->setClearEveryFrame(m_ClearBeforePaint);
}

//////////////////////////////////////////////////////////////////////////
bool Viewport::GetObjectAt(int x, int y, MousePickResult& result)
{
	// TODO expects screen coordinates!!! not viewport coordinates

	float relX = (float)x / (float)m_OgreViewport->getActualWidth();
	float relY = (float)y / (float)m_OgreViewport->getActualHeight();

	ScreenToViewport(x, y);

	reverse_foreach (ActiveSpot* spot, m_ActiveSpots)
	{
		if (spot->QueryObjectAt(x, y, relX, relY, result)) return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
void Viewport::ClearActiveSpots()
{
	foreach (ActiveSpot* spot, m_ActiveSpots)
	{
		SAFE_DELETE(spot);
	}
	m_ActiveSpots.clear();
}

//////////////////////////////////////////////////////////////////////////
void Viewport::AddActiveSpot(ActiveSpot* activeSpot)
{
	m_ActiveSpots.push_back(activeSpot);
}

//////////////////////////////////////////////////////////////////////////
void Viewport::OnRenderQueueStart(Ogre::uint8 renderQueueId)
{
	if (!m_Camera || !m_Camera->GetStage()) return;

	m_Camera->GetStage()->RegisterActiveSpots(this, m_Camera, renderQueueId);
}

//////////////////////////////////////////////////////////////////////////
void Viewport::ScreenToViewport(int& x, int& y) const
{
	x = (int)((float)x / GetScaleX());
	y = (int)((float)y / GetScaleY());
}

//////////////////////////////////////////////////////////////////////////
void Viewport::ViewportToScreen(int& x, int& y) const
{
	x = (int)((float)x * GetScaleX());
	y = (int)((float)y * GetScaleY());
}

//////////////////////////////////////////////////////////////////////////
void Viewport::GetMousePos(int& x, int& y, bool absolute) const
{
	m_ParentLayout->GetParentView()->GetMousePos(x, y);
	x -= m_OgreViewport->getActualLeft();
	y -= m_OgreViewport->getActualTop();
	ScreenToViewport(x, y);

	if (!absolute)
	{
		x += m_ScrollOffsetX;
		y += m_ScrollOffsetY;
	}
}

//////////////////////////////////////////////////////////////////////////
void Viewport::SetMousePos(int x, int y, bool absolute)
{
	if (!absolute)
	{
		x -= m_ScrollOffsetX;
		y -= m_ScrollOffsetY;
	}

	x = (int)(x * GetScaleX());
	y = (int)(y * GetScaleY());

	x += m_OgreViewport->getActualLeft();
	y += m_OgreViewport->getActualTop();

	m_ParentLayout->GetParentView()->SetMousePos(x, y);
}

//////////////////////////////////////////////////////////////////////////
void Viewport::GetRelativeMousePos(float& x, float& y) const
{
	int absX, absY;
	m_ParentLayout->GetParentView()->GetMousePos(absX, absY);

	absX -= m_OgreViewport->getActualLeft();
	absY -= m_OgreViewport->getActualTop();


	x = (float)absX / (float)m_OgreViewport->getActualWidth();
	y = (float)absY / (float)m_OgreViewport->getActualHeight();
}

//////////////////////////////////////////////////////////////////////////
void Viewport::SetRelativeMousePos(float x, float y)
{
	x *= (float)m_OgreViewport->getActualWidth();
	y *= (float)m_OgreViewport->getActualHeight();

	x += m_OgreViewport->getActualLeft();
	y += m_OgreViewport->getActualTop();

	m_ParentLayout->GetParentView()->SetMousePos((int)x, (int)y);
}

//////////////////////////////////////////////////////////////////////////
Ogre::Vector2 Viewport::ConvertMouseToRelative(int absX, int absY)
{
	float relX = (float)absX / (float)m_OgreViewport->getActualWidth();
	float relY = (float)absY / (float)m_OgreViewport->getActualHeight();

	return Ogre::Vector2(relX, relY);
}

//////////////////////////////////////////////////////////////////////////
bool Viewport::GetMousePointOnPlane(const Ogre::Plane& plane, Ogre::Vector3& point)
{
	float x, y;
	GetRelativeMousePos(x, y);

	Ogre::Ray mouseRay = GetCamera()->GetOgreCamera()->getCameraToViewportRay(x, y);
	std::pair<bool, Ogre::Real> hit = mouseRay.intersects(plane);
	if (hit.first)
	{
		point = mouseRay.getPoint(hit.second);
		return true;
	}
	else
	{
		return false;
	}
}

//////////////////////////////////////////////////////////////////////////
bool Viewport::GetMousePointOnRay(const Ogre::Ray& ray, Ogre::Vector3& point)
{
	float x, y;
	GetRelativeMousePos(x, y);
	Ogre::Ray mouseRay = GetCamera()->GetOgreCamera()->getCameraToViewportRay(x, y);

	Ogre::Vector3 p1 = ray.getOrigin();
	Ogre::Vector3 p2 = ray.getPoint(1000.0f);

	Ogre::Vector3 p3 = mouseRay.getOrigin();
	Ogre::Vector3 p4 = mouseRay.getPoint(1000.0f);

	Ogre::Vector3 pa, pb;
	Ogre::Real mua, mub;

	if (MathUtil::LineLineIntersect(p1, p2, p3, p4, pa, pb, mua, mub))
	{
		point = pa;
		return true;
	}
	else return false;
}

//////////////////////////////////////////////////////////////////////////
bool Viewport::IsPointInViewport(int x, int y) const
{
	return	x >= m_OgreViewport->getActualLeft() && x <= m_OgreViewport->getActualLeft() + m_OgreViewport->getActualWidth() &&
			y >= m_OgreViewport->getActualTop() && y <= m_OgreViewport->getActualTop() + m_OgreViewport->getActualHeight();
}

//////////////////////////////////////////////////////////////////////////
bool Viewport::HandleMouseEvent(Viewport* viewport, MouseEvent& event)
{
	// let the controller handle mouse
	if (m_CameraController)
	{
		if (m_CameraController->HandleMouseEvent(event)) return true;
	}

	// let the stage editor handle mouse
	if (m_Camera)
	{
		StageEditor* editor = m_Camera->GetStage()->GetEditor();
		if (editor && editor->HandleMouseEvent(this, event)) return true;
	}

	// route event to stage
	if (m_Camera)
	{
		return m_Camera->GetStage()->HandleMouseEvent(this, event);
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
bool Viewport::HandleKeyboardEvent(KeyboardEvent& event)
{
	// let the controller handle mouse
	if (m_CameraController)
	{
		if (m_CameraController->HandleKeyboardEvent(event)) return true;
	}

	// let the stage editor handle mouse
	if (m_Camera)
	{
		StageEditor* editor = m_Camera->GetStage()->GetEditor();
		if (editor && editor->HandleKeyboardEvent(this, event)) return true;
	}

	// route event to stage
	if (m_Camera)
	{
		return m_Camera->GetStage()->HandleKeyboardEvent(this, event);
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
bool Viewport::HasMouseCapture() const
{
	return (this == Game::GetInstance()->GetContentMgr()->GetMouseCapture());
}

//////////////////////////////////////////////////////////////////////////
void Viewport::PushClippingRect(Rect clippingRect, bool setScissorTest)
{
	m_ClippingRectStack.push_back(m_ClippingRect);
	m_ClippingRect = clippingRect;

	if (setScissorTest)
	{
		TransformClippingRect(clippingRect, true);
		Ogre::RenderSystem* rs = Ogre::Root::getSingleton().getRenderSystem();
		rs->setScissorTest(true, (size_t)clippingRect.GetLeft(), (size_t)clippingRect.GetTop(), (size_t)clippingRect.GetRight(), (size_t)clippingRect.GetBottom());
	}
}

//////////////////////////////////////////////////////////////////////////
void Viewport::PopClippingRect(bool setScissorTest)
{
	m_ClippingRect = m_ClippingRectStack.back();
	m_ClippingRectStack.pop_back();

	if (setScissorTest)
	{
		Ogre::RenderSystem* rs = Ogre::Root::getSingleton().getRenderSystem();

		if (m_ClippingRectStack.empty())
		{
			rs->setScissorTest(false);
		}
		else
		{
			Rect clippingRect = m_ClippingRect;
			TransformClippingRect(clippingRect, true);
			rs->setScissorTest(true, (size_t)clippingRect.GetLeft(), (size_t)clippingRect.GetTop(), (size_t)clippingRect.GetRight(), (size_t)clippingRect.GetBottom());
		}
	}
}


} // namespace Wme
