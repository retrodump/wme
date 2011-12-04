// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "PropMaterial.h"
#include "EntityMaterialWidget.h"
#include "MeshEntity.h"
#include "QtUtil.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
PropMaterial::PropMaterial(QWidget* parent) : PropSectionBase(parent)
{
	m_Entity = NULL;

	m_Layout = new QVBoxLayout(this);
	m_Layout->setMargin(4);	
}

//////////////////////////////////////////////////////////////////////////
PropMaterial::~PropMaterial()
{
	Clear();
}

//////////////////////////////////////////////////////////////////////////
void PropMaterial::Clear()
{
	qDeleteAll(m_Items);
	m_Items.clear();
}

//////////////////////////////////////////////////////////////////////////
void PropMaterial::SetEntity(MeshEntity* ent)
{
	m_Entity = ent;
	DisplayData();
}

//////////////////////////////////////////////////////////////////////////
void PropMaterial::DisplayData()
{
	Clear();

	if (!m_Entity) return;

	Ogre::Entity* ent = m_Entity->GetOgreEntity();
	if (!ent) return;

	for (int i = 0; i < ent->getNumSubEntities(); i++)
	{
		EntityMaterialWidget* matWidget = new EntityMaterialWidget(this);
		m_Layout->addWidget(matWidget);
		m_Items.append(matWidget);

		matWidget->SetSubEntity(m_Entity, i, i == ent->getNumSubEntities() - 1);
		
		connect(matWidget, SIGNAL(RebuildNeeded()), this, SLOT(OnRebuildNeeded()));
	}
}

//////////////////////////////////////////////////////////////////////////
void PropMaterial::OnRebuildNeeded()
{
	// must be delayed so that temporary child objects get deleted first
	// (namely the FilePickerDialog in FileButton)
	QTimer::singleShot(0, this, SLOT(DisplayData()));
}


} // namespace Armed
