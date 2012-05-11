// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "SceneNode2D.h"
#include "Canvas2D.h"
#include "RenderBatch2D.h"
#include "Viewport.h"
#include "MathUtil.h"
#include "RenderModifier.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
SceneNode2D::SceneNode2D(Canvas2D* canvas)
{
	m_Canvas = canvas;

	m_SortNeeded = false;

	m_ParentNode = NULL;
	
	m_ZOrder = 0;
	m_Position = m_DerivedPosition = Ogre::Vector2::ZERO;
	m_Rotation = m_DerivedRotation = 0;
	m_Scale = m_DerivedScale = Ogre::Vector2::UNIT_SCALE;

	m_TransformDirty = true;
	m_GeometryDirty = true;

	m_ClippingRect = Rect::EMPTY_RECT;
	m_Clipper = m_UnClipper = NULL;
	m_ClipChildrenBehind = false;

	m_AttachedElement = NULL;

	m_BatchesUsed = 0;
}

//////////////////////////////////////////////////////////////////////////
SceneNode2D::~SceneNode2D()
{
	DetachElement();

	foreach (RenderBatch2D* batch, m_RenderBatches)
	{
		SAFE_DELETE(batch);
	}

	SAFE_DELETE(m_Clipper);
	SAFE_DELETE(m_UnClipper);
}

//////////////////////////////////////////////////////////////////////////
Viewport* SceneNode2D::GetViewport() const
{
	if (m_Canvas) return m_Canvas->GetViewport();
	else return NULL;
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::AddChild(SceneNode2D* child, int zOrder)
{
	if (!child || child == this) return;
	child->SetParentNode(this);
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::RemoveChild(SceneNode2D* child)
{
	if (!child || child->GetParentNode() != this) return;
	child->SetParentNode(NULL);
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::RemoveAndDestroyChild(SceneNode2D* child, bool destroyElements)
{
	if (!child || child->GetParentNode() != this) return;

	child->RemoveAndDestroyAllChildren(destroyElements);
	if (destroyElements) delete child->GetAttachedElement();
	RemoveChild(child);
	delete child;
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::RemoveAndDestroyAllChildren(bool destroyElements)
{
	foreach (SceneNode2D* child, m_Children)
	{
		child->RemoveAndDestroyAllChildren(destroyElements);
		if (destroyElements) delete child->GetAttachedElement();
		delete child;
	}
	m_Children.clear();
}

//////////////////////////////////////////////////////////////////////////
SceneNode2D* SceneNode2D::CreateChildNode(int zOrder)
{
	SceneNode2D* child = new SceneNode2D(m_Canvas);
	AddChild(child, zOrder);
	return child;
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::SetParentNode(SceneNode2D* newParent)
{
	if (m_ParentNode == newParent) return;

	if (m_ParentNode)
	{
		m_ParentNode->m_Children.remove(this);
	}

	if (newParent)
	{
		newParent->m_Children.push_back(this);
		newParent->m_SortNeeded = true;
		m_ParentNode = newParent;
		SetTransformDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
SceneNode2D* SceneNode2D::GetChild(size_t index) const
{
	SortChildren();

	if (index >= m_Children.size()) return NULL;

	size_t i = 0;
	foreach (SceneNode2D* child, m_Children)
	{
		if (i == index) return child;
		i++;
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::SetZOrder(int zOrder)
{
	if (zOrder != m_ZOrder)
	{
		m_ZOrder = zOrder;
		m_SortNeeded = true;
	}
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::SetPosition(const Ogre::Vector2& pos)
{
	if (m_Position != pos)
	{
		m_Position = pos;
		SetTransformDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::SetPosition(float x, float y)
{
	SetPosition(Ogre::Vector2(x, y));
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::SetRotation(const Ogre::Degree& angle)
{
	if (m_Rotation != angle)
	{
		m_Rotation = MathUtil::NormalizeAngle(angle);
		SetTransformDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::SetRotation(float degrees)
{
	SetRotation(Ogre::Degree(degrees));
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::SetScale(const Ogre::Vector2& scale)
{
	if (m_Scale != scale)
	{
		m_Scale = scale;
		SetTransformDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::SetScale(float x, float y)
{
	SetScale(Ogre::Vector2(x, y));
}

//////////////////////////////////////////////////////////////////////////
const Ogre::Vector2& SceneNode2D::GetDerivedPosition()
{
	UpdateTransform();
	return m_DerivedPosition;
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::SetDerivedPosition(const Ogre::Vector2& pos)
{
	if (!m_ParentNode) SetPosition(pos);
	else SetPosition(m_ParentNode->PositionSceneToLocal(pos));
}

//////////////////////////////////////////////////////////////////////////
const Ogre::Degree& SceneNode2D::GetDerivedRotation()
{
	UpdateTransform();
	return m_DerivedRotation;
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::SetDerivedPosition(float x, float y)
{
	SetDerivedPosition(Ogre::Vector2(x, y));
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::SetDerivedRotation(const Ogre::Degree& angle)
{
	if (!m_ParentNode) SetRotation(angle);
	else SetRotation(m_ParentNode->RotationSceneToLocal(angle));	
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::SetDerivedRotation(float degrees)
{
	SetDerivedRotation(Ogre::Degree(degrees));
}

//////////////////////////////////////////////////////////////////////////
const Ogre::Vector2& SceneNode2D::GetDerivedScale()
{
	UpdateTransform();
	return m_DerivedScale;
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::SetDerivedScale(const Ogre::Vector2& scale)
{
	if (!m_ParentNode) SetScale(scale);
	else SetScale(m_ParentNode->ScaleSceneToLocal(scale));
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::SetDerivedScale(float x, float y)
{
	SetDerivedScale(Ogre::Vector2(x, y));
}

//////////////////////////////////////////////////////////////////////////
const Transform2D& SceneNode2D::GetSceneTransform()
{
	UpdateTransform();
	return m_SceneTransform;
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::SortChildren() const
{
	if (!m_SortNeeded) return;

	m_Children.sort(SceneNode2D::ZOrderComparer);
	
	m_SortNeeded = false;
}

//////////////////////////////////////////////////////////////////////////
bool SceneNode2D::ZOrderComparer(SceneNode2D* node1, SceneNode2D* node2)
{
	return node1->m_ZOrder < node2->m_ZOrder;
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::SetTransformDirty()
{
	m_TransformDirty = true;
	SetGeometryDirty(true);
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::SetGeometryDirty(bool includeChildren)
{
	m_GeometryDirty = true;

	if (includeChildren)
	{
		foreach (SceneNode2D* child, m_Children)
		{
			child->SetGeometryDirty(true);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::AttachElement(Element2D* element)
{
	if (element == m_AttachedElement) return;

	if (m_AttachedElement) DetachElement();
	m_AttachedElement = element;
	if (m_AttachedElement) m_AttachedElement->OnAttached(this);

	SetGeometryDirty();
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::DetachElement()
{
	if (m_AttachedElement)
	{
		m_AttachedElement->OnDetached(this);
		m_AttachedElement = NULL;

		SetGeometryDirty();
	}
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::UpdateTransform(bool forceUpdate, bool updateChildren)
{
	if (!forceUpdate && !m_TransformDirty) return;

	UpdateTransformInternal();

	m_TransformDirty = false;

	if (updateChildren)
	{
		foreach (SceneNode2D* child, m_Children)
		{
			child->UpdateTransform(true, true);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::UpdateTransformInternal()
{
	if (m_ParentNode)
	{
		m_DerivedPosition = m_ParentNode->GetSceneTransform() * m_Position;
		m_DerivedRotation = MathUtil::NormalizeAngle(m_ParentNode->GetDerivedRotation() + m_Rotation);
		m_DerivedScale = m_ParentNode->GetDerivedScale() * m_Scale;
	}
	else
	{
		// root node (no parent)
		m_DerivedPosition = m_Position;
		m_DerivedRotation = m_Rotation;
		m_DerivedScale = m_Scale;
	}

	m_SceneTransform.Reset();
	
	m_SceneTransform.Scale(m_DerivedScale.x, m_DerivedScale.y);	
	m_SceneTransform.Rotate(m_DerivedRotation);
	m_SceneTransform.Translate(m_DerivedPosition.x, m_DerivedPosition.y);
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::UpdateGeometry()
{
	if (!m_GeometryDirty) return;

	m_BoundingRect.Clear();
	m_BatchesUsed = 0;

	// reguest geometry from attached object
	if (m_AttachedElement) m_AttachedElement->AddGeometry();

	// delete unneeded render batches
	while (m_RenderBatches.size() > m_BatchesUsed)
	{
		delete m_RenderBatches.back();
		m_RenderBatches.pop_back();
	}

	m_GeometryDirty = false;
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::AddGeometry(Vertex2D* vertexData, size_t numVerts, const Ogre::MaterialPtr& material, Ogre::RenderOperation::OperationType operType)
{
	RenderBatch2D* batch = GetFreeRenderBatch();
	batch->SetVertices(GetViewport(), vertexData, numVerts, GetSceneTransform(), material, operType, m_BoundingRect);
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::AddGeometry(Vertex2DTex* vertexData, size_t numVerts, const Ogre::MaterialPtr& material, Ogre::RenderOperation::OperationType operType)
{
	RenderBatch2D* batch = GetFreeRenderBatch();
	batch->SetVertices(GetViewport(), vertexData, numVerts, GetSceneTransform(), material, operType, m_BoundingRect);
}

//////////////////////////////////////////////////////////////////////////
RenderBatch2D* SceneNode2D::GetFreeRenderBatch()
{
	size_t counter = 0;
	foreach (RenderBatch2D* batch, m_RenderBatches)
	{
		if (counter == m_BatchesUsed)
		{
			m_BatchesUsed++;
			return batch;
		}
	}

	m_BatchesUsed++;
	RenderBatch2D* newBatch = new RenderBatch2D;
	m_RenderBatches.push_back(newBatch);

	return newBatch;
}

//////////////////////////////////////////////////////////////////////////
Ogre::Vector2 SceneNode2D::PositionSceneToLocal(const Ogre::Vector2& pos) const
{
	return m_SceneTransform.Inverted() * pos;
}

//////////////////////////////////////////////////////////////////////////
Ogre::Degree SceneNode2D::RotationSceneToLocal(const Ogre::Degree& angle) const
{
	return MathUtil::NormalizeAngle(angle - m_Rotation);
}

//////////////////////////////////////////////////////////////////////////
Ogre::Vector2 SceneNode2D::ScaleSceneToLocal(const Ogre::Vector2& scale) const
{
	return scale / m_Scale;
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::Render(Ogre::RenderQueue* renderQueue, byte queueId, word& priority)
{
	SortChildren();
	UpdateTransform();
	UpdateGeometry();
	
	bool renderedSelf = false;
	int prevZOrder = -1;

	if (m_ClipChildrenBehind) StartClipping(renderQueue, queueId, priority);
	
	foreach (SceneNode2D* child, m_Children)
	{
		if (child->GetZOrder() >= 0 && prevZOrder < 0)
		{
			RenderSelf(renderQueue, queueId, priority);
			renderedSelf = true;
		}
		child->Render(renderQueue, queueId, priority);
		prevZOrder = child->GetZOrder();
	}
	if (!renderedSelf) RenderSelf(renderQueue, queueId, priority);

	StopClipping(renderQueue, queueId, priority);
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::RenderSelf(Ogre::RenderQueue* renderQueue, byte queueId, word& priority)
{
	if (!m_ClipChildrenBehind) StartClipping(renderQueue, queueId, priority);

	if (m_AttachedElement && !m_AttachedElement->IsVisible()) return;

	foreach (RenderBatch2D* batch, m_RenderBatches)
	{
		priority++;
		renderQueue->addRenderable(batch, queueId, priority);
	}
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::VisitRenderables(Ogre::Renderable::Visitor* visitor)
{
	foreach (RenderBatch2D* batch, m_RenderBatches)
	{
		visitor->visit(batch, 0, false);
	}

	foreach (SceneNode2D* child, m_Children)
	{
		child->VisitRenderables(visitor);
	}
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::GetElementsAt(float x, float y, const Rect& clippingRect, Element2DList& elements)
{
	bool testedSelf = false;
	int prevZOrder = -1;

	Rect selfClipRect = GetTransformedClippingRect();
	if (selfClipRect.IsEmpty()) selfClipRect = clippingRect;
	else selfClipRect = clippingRect.GetIntersection(selfClipRect);


	foreach (SceneNode2D* child, m_Children)
	{
		if (child->GetZOrder() >= 0 && prevZOrder < 0)
		{
			if (HitTest(x, y, selfClipRect)) elements.push_back(m_AttachedElement);
			testedSelf = true;
		}

		bool clipChild = (m_ClipChildrenBehind && child->GetZOrder() < 0) || child->GetZOrder() >= 0;

		child->GetElementsAt(x, y, clipChild ? selfClipRect : clippingRect, elements);
		prevZOrder = child->GetZOrder();
	}
	if (!testedSelf)
	{
		if (HitTest(x, y, selfClipRect)) elements.push_back(m_AttachedElement);
	}
}

//////////////////////////////////////////////////////////////////////////
bool SceneNode2D::HitTest(float x, float y, const Rect& clippingRect) const
{
	if (!m_AttachedElement || !m_AttachedElement->IsVisible()) return false;
	if (!clippingRect.IsEmpty() && !clippingRect.ContainsPoint(x, y)) return false;
	if (!m_BoundingRect.ContainsPoint(x, y)) return false;

	Ogre::Vector2 localPos = PositionSceneToLocal(Ogre::Vector2(x, y));
	return !m_AttachedElement->IsTransparentAt(localPos.x, localPos.y);
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::SetClippingRect(const Rect& rect)
{
	m_ClippingRect = rect;
	if (!m_ClippingRect.IsEmpty())
	{
		if (!m_Clipper) m_Clipper = new RenderModifier();
		if (!m_UnClipper) m_UnClipper = new RenderModifier();
	}
}

//////////////////////////////////////////////////////////////////////////
Rect SceneNode2D::GetTransformedClippingRect()
{
	if (m_ClippingRect.IsEmpty() || fmod(GetDerivedRotation().valueDegrees(), 90.0f) != 0.0f) return Rect();
	else
	{
		return GetSceneTransform() * m_ClippingRect;
	}
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::StartClipping(Ogre::RenderQueue* renderQueue, byte queueId, word& priority)
{
	// we only support clipping rectanges rotated by multiplies of 90 degrees
	if (m_ClippingRect.IsEmpty() || fmod(GetDerivedRotation().valueDegrees(), 90.0f) != 0.0f) return;
		
	m_Clipper->SetClipping(true, m_Canvas->GetViewport(), m_Canvas->GetViewport()->GetClippingRect().GetIntersection(GetTransformedClippingRect()));

	priority++;
	renderQueue->addRenderable(m_Clipper, queueId, priority);
}

//////////////////////////////////////////////////////////////////////////
void SceneNode2D::StopClipping(Ogre::RenderQueue* renderQueue, byte queueId, word& priority)
{
	if (m_ClippingRect.IsEmpty() || fmod(GetDerivedRotation().valueDegrees(), 90.0f) != 0.0f) return;

	m_UnClipper->SetClipping(false, m_Canvas->GetViewport());

	priority++;
	renderQueue->addRenderable(m_UnClipper, queueId, priority);
}


} // namespace Wme
