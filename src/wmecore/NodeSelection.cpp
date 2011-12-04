// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "NodeSelection.h"
#include "CompositeBBox.h"
#include "Game.h"
#include "StringUtil.h"
#include "SceneNodeEditor.h"
#include "MaterialUtil.h"
#include "Entity3DBase.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
NodeSelection::NodeSelection(SceneNodeEditor* editor)
{
	m_Editor = editor;

	m_BBox = NULL;
	m_Origin = Ogre::Vector3::ZERO;
	m_IsBulkSelecting = false;
}

//////////////////////////////////////////////////////////////////////////
NodeSelection::~NodeSelection()
{
	SAFE_DELETE(m_BBox);
}

//////////////////////////////////////////////////////////////////////////
void NodeSelection::StartBulkSelection()
{
	m_IsBulkSelecting = true;
}

//////////////////////////////////////////////////////////////////////////
void NodeSelection::EndBulkSelection()
{
	if (m_IsBulkSelecting)
	{
		m_IsBulkSelecting = false;
		RebuildBBox();
		EmitSelectionChanged();
	}
}

//////////////////////////////////////////////////////////////////////////
void NodeSelection::Refresh()
{
	RebuildBBox();
}

//////////////////////////////////////////////////////////////////////////
void NodeSelection::AddNode(Ogre::SceneNode* node)
{
	RemoveNode(node);
	HighlightNode(node, true);
	m_Nodes.push_back(node);

	if (!m_IsBulkSelecting)
	{
		RebuildBBox();
		EmitSelectionChanged();
	}
}

//////////////////////////////////////////////////////////////////////////
void NodeSelection::Clear(bool emitEvent)
{
	foreach (Ogre::SceneNode* node, m_Nodes)
	{
		HighlightNode(node, false);
	}
	m_Nodes.clear();
	if (emitEvent) EmitSelectionChanged();
}

//////////////////////////////////////////////////////////////////////////
void NodeSelection::SetNode(Ogre::SceneNode* node)
{
	Clear(false);

	HighlightNode(node, true);
	m_Nodes.push_back(node);

	RebuildBBox();
	EmitSelectionChanged();
}

//////////////////////////////////////////////////////////////////////////
void NodeSelection::RemoveNode(Ogre::SceneNode* node)
{
	HighlightNode(node, false);
	m_Nodes.remove(node);

	if (!m_IsBulkSelecting)
	{
		RebuildBBox();
		EmitSelectionChanged();
	}
}

//////////////////////////////////////////////////////////////////////////
bool NodeSelection::IsSelected(Ogre::SceneNode* node)
{
	foreach (Ogre::SceneNode* testNode, m_Nodes)
	{
		if (testNode == node) return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool NodeSelection::IsEmpty() const
{
	return m_Nodes.empty();
}

//////////////////////////////////////////////////////////////////////////
bool NodeSelection::IsMultiSelect() const
{
	return m_Nodes.size() > 1;
}

//////////////////////////////////////////////////////////////////////////
const Ogre::Vector3& NodeSelection::GetPosition()
{
	if (m_Nodes.size() == 0) return Ogre::Vector3::ZERO;
	else return m_Nodes.back()->_getDerivedPosition();
}

//////////////////////////////////////////////////////////////////////////
const Ogre::Quaternion& NodeSelection::GetOrientation()
{
	if (m_Nodes.size() == 0) return Ogre::Quaternion::IDENTITY;
	else return m_Nodes.back()->_getDerivedOrientation();
}

//////////////////////////////////////////////////////////////////////////
const Ogre::Vector3& NodeSelection::GetScale()
{
	if (m_Nodes.size() == 0) return Ogre::Vector3::UNIT_SCALE;
	else return m_Nodes.back()->_getDerivedScale();
}

//////////////////////////////////////////////////////////////////////////
void NodeSelection::SetOrientation(const Ogre::Quaternion& q, bool local, bool emitEvent)
{
	// no love for autotracking nodes
	if (local)
	{
		foreach (Ogre::SceneNode* node, m_Nodes)
		{
			if (node->getAutoTrackTarget()) return;
		}
	}


	Ogre::SceneNode* anchor = GetAnchorNode();
	if (!anchor) return;

	Ogre::Quaternion rotDiff = q * anchor->_getDerivedOrientation().Inverse(); 
	Ogre::Vector3 origin = GetOrigin();

	foreach (Ogre::SceneNode* node, m_Nodes)
	{
		if (!m_Editor->IsNodeRotatable(node)) continue;

		Ogre::Vector3 pos = node->_getDerivedPosition();
		Ogre::Quaternion rot = node->_getDerivedOrientation();

		Ogre::Vector3 newPos = (rotDiff * (pos - origin)) + origin;
		node->_setDerivedPosition(newPos);
		node->_setDerivedOrientation(rotDiff * rot);
	}

	RebuildBBox();

	if (emitEvent && rotDiff != Ogre::Quaternion::IDENTITY) EmitOrientationChanged();
}

//////////////////////////////////////////////////////////////////////////
void NodeSelection::ScaleBy(const Ogre::Vector3& scaleDiff, bool emitEvent)
{
	if (scaleDiff == Ogre::Vector3::ZERO) return;

	foreach (Ogre::SceneNode* node, m_Nodes)
	{
		if (!m_Editor->IsNodeScalable(node)) continue;

		Ogre::Vector3 scale = node->getScale();
		Ogre::Vector3 pos = node->_getDerivedPosition();
		
		// adjust scale
		Ogre::Vector3 newScale = scale * scaleDiff;
		if (newScale.x == 0.0f) newScale.x = 0.000001f;
		if (newScale.y == 0.0f) newScale.y = 0.000001f;
		if (newScale.z == 0.0f) newScale.z = 0.000001f;

		node->setScale(scale * scaleDiff);


		// adjust position (off-center scale)
		Ogre::Quaternion rot = node->_getDerivedOrientation();
		Ogre::Vector3 newPos = pos - GetScaleOrigin();

		Ogre::Vector3 axisX = rot * Ogre::Vector3::UNIT_X;
		Ogre::Vector3 axisY = rot * Ogre::Vector3::UNIT_Y;
		Ogre::Vector3 axisZ = rot * Ogre::Vector3::UNIT_Z;

		Ogre::Vector3 vPos1 = (axisX.dotProduct(newPos) * axisX);
		Ogre::Vector3 vPos2 = (axisY.dotProduct(newPos) * axisY);
		Ogre::Vector3 vPos3 = (axisZ.dotProduct(newPos) * axisZ);
		newPos = (vPos1 * scaleDiff.x) + (vPos2 * scaleDiff.y) + (vPos3 * scaleDiff.z) + GetScaleOrigin();

		node->_setDerivedPosition(newPos);
	}
	RebuildBBox();

	if (emitEvent && scaleDiff != Ogre::Vector3::ZERO) EmitScaleChanged();
}

//////////////////////////////////////////////////////////////////////////
void NodeSelection::MoveBy(const Ogre::Vector3& moveDiff, bool local, bool emitEvent)
{
	// no love for autotracking nodes
	if (local)
	{
		foreach (Ogre::SceneNode* node, m_Nodes)
		{
			if (node->getAutoTrackTarget()) return;
		}
	}


	foreach (Ogre::SceneNode* node, m_Nodes)
	{
		if (!m_Editor->IsNodeMovable(node)) continue;

		Ogre::Vector3 pos = node->_getDerivedPosition();
		node->_setDerivedPosition(pos + moveDiff);
	}
	RebuildBBox();

	if (emitEvent && moveDiff != Ogre::Vector3::ZERO) EmitPositionChanged();
}

//////////////////////////////////////////////////////////////////////////
void NodeSelection::HighlightNode(Ogre::SceneNode* node, bool highlight)
{
	Entity3DBase* entity = Entity3DBase::NodeToEntity(node);
	if (entity)
	{
		Ogre::Entity* ogreEnt = entity->GetSelectableEntity();
		if (ogreEnt) HighlightEntity(ogreEnt, highlight);
		else return;
	}

	for (unsigned short i = 0; i < node->numAttachedObjects(); i++)
	{
		Ogre::MovableObject* obj = node->getAttachedObject(i);
		if (obj->getMovableType() == "Entity")
			HighlightEntity(static_cast<Ogre::Entity*>(obj), highlight);
	}
}

//////////////////////////////////////////////////////////////////////////
void NodeSelection::HighlightEntity(Ogre::Entity* entity, bool highlight)
{
	for (size_t i = 0; i < entity->getNumSubEntities(); i++)
	{
		Ogre::SubEntity* sub = entity->getSubEntity(i);

		// select
		if (highlight)
		{
			if (MaterialUtil::IsHighlightedName(sub->getMaterialName())) continue;

			Ogre::MaterialPtr hlMat = MaterialUtil::GetHighlightMat(sub->getMaterial());
			sub->setMaterial(hlMat);
		}
		// deselect
		else
		{
			if (MaterialUtil::IsHighlightedName(sub->getMaterialName()))
			{
				sub->setMaterialName(MaterialUtil::GetUnhighlightedName(sub->getMaterialName()));
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
Ogre::SceneNode* NodeSelection::GetAnchorNode()
{
	if (m_Nodes.empty()) return NULL;
	else return m_Nodes.back();
}

//////////////////////////////////////////////////////////////////////////
void NodeSelection::RebuildBBox()
{
	if (!m_BBox) m_BBox = new CompositeBBox();

	m_BBox->Clear();

	reverse_foreach (Ogre::SceneNode* node, m_Nodes)
	{
		bool hasMesh = false;
		for (unsigned short i = 0; i < node->numAttachedObjects(); i++)
		{
			Ogre::MovableObject* obj = node->getAttachedObject(i);
			if (obj->getMovableType() == "Entity")
			{
				m_BBox->AddEntity(static_cast<Ogre::Entity*>(obj));
				hasMesh = true;
			}
		}
		if (!hasMesh) m_BBox->AddNode(node);
	}

	// origin point
	Ogre::Vector3 min, max;
	min = GetBBox()->GetMinimum();
	max = GetBBox()->GetMaximum();

	float midX = min.x + (max.x - min.x) / 2;
	float midY = min.y + (max.y - min.y) / 2;
	float midZ = min.z + (max.z - min.z) / 2;

	m_Origin = Ogre::Vector3(midX, midY, midZ);

	GetBBox()->LocalToWorld(m_Origin);
}

//////////////////////////////////////////////////////////////////////////
const Ogre::Vector3& NodeSelection::GetOrigin()
{
	if (m_Nodes.empty()) return Ogre::Vector3::ZERO;
	else if (m_Nodes.size() == 1) return m_Nodes.back()->_getDerivedPosition();
	else return GetScaleOrigin();
}

//////////////////////////////////////////////////////////////////////////
const Ogre::Vector3& NodeSelection::GetScaleOrigin()
{
	return m_Origin;
}

//////////////////////////////////////////////////////////////////////////
void NodeSelection::AddListener(Listener* listener)
{
	if (std::find(m_Listeners.begin(), m_Listeners.end(), listener) == m_Listeners.end())
		m_Listeners.push_back(listener);
}

//////////////////////////////////////////////////////////////////////////
void NodeSelection::RemoveListener(Listener* listener)
{
	ListenerList::iterator it = std::find(m_Listeners.begin(), m_Listeners.end(), listener);
	if (it != m_Listeners.end()) m_Listeners.erase(it);
}

//////////////////////////////////////////////////////////////////////////
void NodeSelection::EmitSelectionChanged()
{
	foreach (Listener* listener, m_Listeners)
	{
		listener->NodeSelectionChanged();
	}
}

//////////////////////////////////////////////////////////////////////////
void NodeSelection::EmitPositionChanged()
{
	foreach (Listener* listener, m_Listeners)
	{
		listener->NodeSelectionPositionChanged();
	}
}

//////////////////////////////////////////////////////////////////////////
void NodeSelection::EmitOrientationChanged()
{
	foreach (Listener* listener, m_Listeners)
	{
		listener->NodeSelectionOrientationChanged();
	}
}

//////////////////////////////////////////////////////////////////////////
void NodeSelection::EmitScaleChanged()
{
	foreach (Listener* listener, m_Listeners)
	{
		listener->NodeSelectionScaleChanged();
	}
}

//////////////////////////////////////////////////////////////////////////
Ogre::Vector3 NodeSelection::GetCenter()
{
	Ogre::Vector3 min, max;
	min = GetBBox()->GetMinimum();
	max = GetBBox()->GetMaximum();

	float midX = min.x + (max.x - min.x) / 2;
	float midY = min.y + (max.y - min.y) / 2;
	float midZ = min.z + (max.z - min.z) / 2;

	Ogre::Vector3 center = Ogre::Vector3(midX, midY, midZ);
	GetBBox()->LocalToWorld(center);

	return center;
}

//////////////////////////////////////////////////////////////////////////
Ogre::Vector3 NodeSelection::GetHalfSize()
{
	return (GetBBox()->GetMaximum() - GetBBox()->GetMinimum()) * 0.5;
}

//////////////////////////////////////////////////////////////////////////
bool NodeSelection::IsMovable() const
{
	foreach (Ogre::SceneNode* node, m_Nodes)
	{
		if (!m_Editor->IsNodeMovable(node)) return false;
	}
	return !IsEmpty();
}

//////////////////////////////////////////////////////////////////////////
bool NodeSelection::IsRotatable() const
{
	foreach (Ogre::SceneNode* node, m_Nodes)
	{
		if (!m_Editor->IsNodeRotatable(node)) return false;
	}
	return !IsEmpty();
}

//////////////////////////////////////////////////////////////////////////
bool NodeSelection::IsScalable() const
{
	foreach (Ogre::SceneNode* node, m_Nodes)
	{
		if (!m_Editor->IsNodeScalable(node)) return false;
	}
	return !IsEmpty();
}


} // namespace Wme
