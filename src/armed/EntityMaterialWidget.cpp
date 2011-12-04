// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "EntityMaterialWidget.h"
#include "MeshEntity.h"
#include "FileButton.h"
#include "QtUtil.h"
#include "MaterialInstance.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
EntityMaterialWidget::EntityMaterialWidget(QWidget* parent) : MaterialWidget(parent)
{
	m_Entity = NULL;
	m_SubEnt = 0;
}

//////////////////////////////////////////////////////////////////////////
EntityMaterialWidget::~EntityMaterialWidget()
{

}


//////////////////////////////////////////////////////////////////////////
void EntityMaterialWidget::SetSubEntity(MeshEntity* ent, int subEnt, bool isLast)
{
	m_Entity = ent;
	m_SubEnt = subEnt;	

	m_Material = GetCurrentMaterial();
	m_IgnoreEvents = true;
	if (m_Material)
	{
		m_MatButton->SetFileName(QtUtil::WideToQString(m_Material->GetFileName()));
		BuildPropGui();
	}
	else
	{
		//m_MatButton->SetFileName(QString::fromUtf8(ent->getSubEntity(subEnt)->getMaterialName().c_str()));
		m_MatButton->SetFileName("");
	}
	SetState();

	m_IgnoreEvents = false;


	if (!isLast)
	{
		QFrame* line = new QFrame(this);
		line->setFrameShape(QFrame::HLine);
		line->setFrameShadow(QFrame::Sunken);
		m_Layout->addWidget(line);
	}
}

//////////////////////////////////////////////////////////////////////////
MaterialInstance* EntityMaterialWidget::GetCurrentMaterial()
{
	if (m_Entity)
	{
		return m_Entity->GetMaterial(m_SubEnt);
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
void EntityMaterialWidget::ResetMaterial(const QString& fileName)
{
	if (m_Entity)
	{
		m_Entity->SetMaterial(QtUtil::QStringToWide(fileName), m_SubEnt);
	}
	m_Material = GetCurrentMaterial();
}

//////////////////////////////////////////////////////////////////////////
void EntityMaterialWidget::ReApplyMaterial()
{
	if (m_Entity)
	{
		m_Entity->ReApplyMaterial(m_SubEnt);
	}
}


} // namespace Armed
