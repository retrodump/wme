// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeViewport_H__
#define __WmeViewport_H__


#include "InteractiveObject.h"
#include "IMouseHandler.h"
#include "KeyboardEvent.h"
#include "MousePickParams.h"
#include "Rect.h"


namespace Wme
{

	class Ogre::Viewport;
	class View;
	class Camera;
	class CameraController;
	class ViewportLayout;
	class ScriptableObject;
	class ActiveSpot;


	class WmeDllExport Viewport : public InteractiveObject
	{
	public:

		Viewport();
		virtual ~Viewport();

		void TransformVertices(SpriteVertex* vertices, size_t numVertices, bool absolutePos = false);
		void RotateVertices(SpriteVertex* verts, size_t numVerts, const Ogre::Vector2& centerPoint, float angle);

		void TransformClippingRect(Rect& rect, bool absolutePos);

		void SetDimensions(size_t designWidth, size_t designHeight, size_t virtualWidth = 0, size_t virtualHeight = 0);
		void SetScrollOffsetX(int newOffset);
		void SetScrollOffsetY(int newOffset);

		ViewportLayout* GetParentLayout() const { return m_ParentLayout; }
		View* GetParentView() const;
		Ogre::Viewport* GetOgreViewport() const { return m_OgreViewport; }

		void SetCamera(Camera* camera);
		Camera* GetCamera() const { return m_Camera; }

		void SetCameraController(CameraController* controller);
		CameraController* GetCameraController() const { return m_CameraController; }

		void Create(ViewportLayout* parentLayout, Camera* camera, float left = 0.0f, float top = 0.0f, float width = 1.0f, float height = 1.0f);
		void Update();

		void PreDisplay();
		void PostDisplay();

		void OnResize();

		void ReleaseOgreViewports();
		void RebuildOgreViewports(int& zOrder);

		WideString GetName() const { return m_Name; }
		void SetName(const WideString& name) { m_Name = name; }

		bool IsClickThrough() { return m_IsClickThrough; }
		void SetClickThrough(bool val) { m_IsClickThrough = val; }

		bool GetObjectAt(int x, int y, MousePickResult& result);

		void ClearActiveSpots();
		void AddActiveSpot(ActiveSpot* activeSpot);

		void OnRenderQueueStart(Ogre::uint8 renderQueueId);

		int GetScrollOffsetX() const { return m_ScrollOffsetX; }
		int GetScrollOffsetY() const { return m_ScrollOffsetY; }

		size_t GetDesignWidth() const { return m_DesignWidth; }
		size_t GetDesignHeight() const { return m_DesignHeight; }

		size_t GetVirtualWidth() const { return m_VirtualWidth; }
		size_t GetVirtualHeight() const { return m_VirtualHeight; }

		float GetScaleX() const;
		float GetScaleY() const;

		void SetDirty() { m_UpdateId++; }
		bool IsDirty(int updateId) const { return updateId != m_UpdateId; }
		int GetUpdateId() const { return m_UpdateId; }

		void SetAutoScale(bool val) { m_AutoScale = val; }
		bool GetAutoScale() const { return m_AutoScale; }

		void SetScaleFactor(float val) { m_ScaleFactor = val; }
		float GetScaleFactor() const { return m_ScaleFactor; }

		bool IsPointInViewport(int x, int y) const;

		void ScreenToViewport(int& x, int& y) const;
		void ViewportToScreen(int& x, int& y) const;
		void GetMousePos(int& x, int& y, bool absolute = true) const;
		void SetMousePos(int x, int y, bool absolute = true);

		void GetRelativeMousePos(float& x, float& y) const;
		void SetRelativeMousePos(float x, float y);

		Ogre::Vector2 ConvertMouseToRelative(int absX, int absY);

		bool GetMousePointOnPlane(const Ogre::Plane& plane, Ogre::Vector3& point);
		bool GetMousePointOnRay(const Ogre::Ray& ray, Ogre::Vector3& point);

		Rect GetClippingRect() const { return m_ClippingRect; }
		void PushClippingRect(Rect clippingRect, bool setScissorTest);
		void PopClippingRect(bool setScissorTest);

		bool GetClearBeforePaint() const { return m_ClearBeforePaint; }
		void SetClearBeforePaint(bool val);

        // ScriptableObject
		RTTI(Viewport);
		virtual WideString ConvertToString() { return L"Viewport object"; }
		virtual WideString GetTypeName() const { return L"Viewport"; }

		// events
		bool HandleMouseEvent(Viewport* viewport, MouseEvent& event);
		bool HasMouseCapture() const;
		bool HandleKeyboardEvent(KeyboardEvent& event);
		
	private:
		int m_UpdateId;
		WideString m_Name;

		Camera* m_Camera;
		CameraController* m_CameraController;

		ViewportLayout* m_ParentLayout;
		void DetachFromView();

		float m_ScaleFactor;
		bool m_AutoScale;

		bool m_IsClickThrough;
		bool m_ClearBeforePaint;

		size_t m_DesignWidth;
		size_t m_DesignHeight;
		size_t m_VirtualWidth;
		size_t m_VirtualHeight;
		int m_ScrollOffsetX;
		int m_ScrollOffsetY;

		// viewport position on screen
		float m_RelativeLeft;
		float m_RelativeTop;
		float m_RelativeWidth;
		float m_RelativeHeight;

		Ogre::Viewport* m_OgreViewport;

		typedef std::list<ActiveSpot*> ActiveSpotList;
		ActiveSpotList m_ActiveSpots;

		bool m_IsRendering;

		Rect m_ClippingRect;
		std::list<Rect> m_ClippingRectStack;
	};
}


#endif // __WmeViewport_H__
