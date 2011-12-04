// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "PropSkybox.h"
#include "SkyboxMaterialWidget.h"
#include "Scene3DBase.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
PropSkybox::PropSkybox(QWidget* parent) : PropSectionBase(parent)
{
	m_Scene = NULL;
	m_MaterialWidget = NULL;

	m_Layout = new QVBoxLayout(this);
	m_Layout->setMargin(4);	
}

//////////////////////////////////////////////////////////////////////////
PropSkybox::~PropSkybox()
{

}

//////////////////////////////////////////////////////////////////////////
void PropSkybox::SetScene(Scene3DBase* scene)
{
	m_Scene = scene;
	DisplayData();
}

//////////////////////////////////////////////////////////////////////////
void PropSkybox::DisplayData()
{
	SAFE_DELETE(m_MaterialWidget);
	if (!m_Scene) return;

	m_MaterialWidget = new SkyboxMaterialWidget(this);
	m_Layout->addWidget(m_MaterialWidget);

	m_MaterialWidget->SetScene(m_Scene);

	connect(m_MaterialWidget, SIGNAL(RebuildNeeded()), this, SLOT(OnRebuildNeeded()));
}

//////////////////////////////////////////////////////////////////////////
void PropSkybox::OnRebuildNeeded()
{
	// must be delayed so that temporary child objects get deleted first
	// (namely the FilePickerDialog in FileButton)
	QTimer::singleShot(0, this, SLOT(DisplayData()));
}


} // namespace Armed
