// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "SkyboxMaterialWidget.h"
#include "Scene3DBase.h"
#include "FileButton.h"
#include "QtUtil.h"
#include "MaterialInstance.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
SkyboxMaterialWidget::SkyboxMaterialWidget(QWidget* parent) : MaterialWidget(parent)
{
	m_Scene = NULL;
}

//////////////////////////////////////////////////////////////////////////
SkyboxMaterialWidget::~SkyboxMaterialWidget()
{

}

//////////////////////////////////////////////////////////////////////////
void SkyboxMaterialWidget::SetScene(Scene3DBase* scene)
{
	m_Scene = scene;
	m_Material = GetCurrentMaterial();
	
	m_IgnoreEvents = true;
	if (m_Material)
	{
		m_MatButton->SetFileName(QtUtil::WideToQString(m_Material->GetFileName()));
		BuildPropGui();
	}
	else
	{
		m_MatButton->SetFileName("");
	}
	SetState();

	m_IgnoreEvents = false;
}

//////////////////////////////////////////////////////////////////////////
MaterialInstance* SkyboxMaterialWidget::GetCurrentMaterial()
{
	if (m_Scene)
	{
		return m_Scene->GetSkyboxMaterial();
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
void SkyboxMaterialWidget::ResetMaterial(const QString& fileName)
{
	if (m_Scene)
	{
		m_Scene->SetSkyboxMaterial(QtUtil::QStringToWide(fileName));
	}
	m_Material = GetCurrentMaterial();
}

//////////////////////////////////////////////////////////////////////////
void SkyboxMaterialWidget::ReApplyMaterial()
{
	if (m_Scene)
	{
		m_Scene->ReApplySkyboxMaterial();
	}
}


} // namespace Armed
