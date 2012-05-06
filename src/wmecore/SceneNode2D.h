// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeSceneNode2D_H__
#define __WmeSceneNode2D_H__


#include "Transform2D.h"
#include "BoundingRect.h"
#include "Element2D.h"
#include "Rect.h"


namespace Wme
{
	class Canvas2D;
	class RenderBatch2D;
	class Viewport;
	class RenderModifier;


	class WmeDllExport SceneNode2D
	{
	public:
		SceneNode2D(Canvas2D* canvas);
		virtual ~SceneNode2D();

		typedef std::list<SceneNode2D*> NodeList;

		Canvas2D* GetCanvas() const { return m_Canvas; }
		Viewport* GetViewport() const;

		void AddChild(SceneNode2D* child, int zOrder = 0);
		void RemoveChild(SceneNode2D* child);

		SceneNode2D* GetParentNode() const { return m_ParentNode; }
		void SetParentNode(SceneNode2D* newParent);

		SceneNode2D* CreateChildNode(int zOrder = 0);

		size_t GetNumChildren() const { return m_Children.size(); }
		SceneNode2D* GetChild(size_t index) const;

		int GetZOrder() const { return m_ZOrder; }
		void SetZOrder(int zOrder);

		const Ogre::Vector2& GetPosition() const { return m_Position; }
		void SetPosition(const Ogre::Vector2& pos);
		void SetPosition(float x, float y);

		const Ogre::Degree& GetRotation() const { return m_Rotation; }
		void SetRotation(const Ogre::Degree& angle);
		void SetRotation(float degrees);

		const Ogre::Vector2& GetScale() const { return m_Scale; }
		void SetScale(const Ogre::Vector2& scale);
		void SetScale(float x, float y);

		const Ogre::Vector2& GetDerivedPosition();
		void SetDerivedPosition(const Ogre::Vector2& pos);
		void SetDerivedPosition(float x, float y);

		const Ogre::Degree& GetDerivedRotation();
		void SetDerivedRotation(const Ogre::Degree& angle);
		void SetDerivedRotation(float degrees);

		const Ogre::Vector2& GetDerivedScale();
		void SetDerivedScale(const Ogre::Vector2& scale);
		void SetDerivedScale(float x, float y);

		const Transform2D& GetSceneTransform();
		
		const BoundingRect& GetBoundingRect() const { return m_BoundingRect; }

		void AttachElement(Element2D* element);
		void DetachElement();
		Element2D* GetAttachedElement() const { return m_AttachedElement; }

		Ogre::Vector2 PositionSceneToLocal(const Ogre::Vector2& pos) const;
		Ogre::Degree RotationSceneToLocal(const Ogre::Degree& angle) const;
		Ogre::Vector2 ScaleSceneToLocal(const Ogre::Vector2& scale) const;

		void Render(Ogre::RenderQueue* renderQueue, byte queueId, word& priority);
		void VisitRenderables(Ogre::Renderable::Visitor* visitor);

		void AddGeometry(Vertex2D* vertexData, size_t numVerts, const Ogre::MaterialPtr& material, Ogre::RenderOperation::OperationType operType);
		void AddGeometry(Vertex2DTex* vertexData, size_t numVerts, const Ogre::MaterialPtr& material, Ogre::RenderOperation::OperationType operType);

		void GetElementsAt(float x, float y, const Rect& clippingRect, Element2DList& elements);

		void SetTransformDirty();
		void SetGeometryDirty(bool includeChildren = false);

		void SetClippingRect(const Rect& rect);
		const Rect& GetClippingRect() const { return m_ClippingRect; }

		void SetClipChildrenBehind(bool val) { m_ClipChildrenBehind = val; }
		bool GetClipChildrenBehind() const { return m_ClipChildrenBehind; }

	private:
		Canvas2D* m_Canvas;

		mutable NodeList m_Children;
		SceneNode2D* m_ParentNode;
		
		int m_ZOrder;
		Ogre::Vector2 m_Position;
		Ogre::Degree m_Rotation;
		Ogre::Vector2 m_Scale;

		Ogre::Vector2 m_DerivedPosition;
		Ogre::Degree m_DerivedRotation;
		Ogre::Vector2 m_DerivedScale;

		Transform2D m_SceneTransform;

		BoundingRect m_BoundingRect;

		mutable bool m_SortNeeded;
		void SortChildren() const;
		static bool ZOrderComparer(SceneNode2D* node1, SceneNode2D* node2);

		bool m_TransformDirty;
		bool m_GeometryDirty;

		void UpdateTransform(bool forceUpdate = false, bool updateChildren = true);
		void UpdateTransformInternal();

		void UpdateGeometry();
		size_t m_BatchesUsed;

		Element2D* m_AttachedElement;

		typedef std::list<RenderBatch2D*> RenderBatchList;
		RenderBatchList m_RenderBatches;

		void RenderSelf(Ogre::RenderQueue* renderQueue, byte queueId, word& priority);

		RenderBatch2D* GetFreeRenderBatch();

		bool HitTest(float x, float y, const Rect& clippingRect) const;

		Rect m_ClippingRect;
		RenderModifier* m_Clipper;
		RenderModifier* m_UnClipper;

		bool m_ClipChildrenBehind;

		void StartClipping(Ogre::RenderQueue* renderQueue, byte queueId, word& priority);
		void StopClipping(Ogre::RenderQueue* renderQueue, byte queueId, word& priority);
		Rect GetTransformedClippingRect();
	};
}

#endif // __WmeSceneNode2D_H__
