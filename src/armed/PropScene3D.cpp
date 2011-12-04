// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "PropScene3D.h"
#include "Scene3DBase.h"
#include "QtUtil.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
PropScene3D::PropScene3D(QWidget* parent) : PropSectionBase(parent)
{
	m_IsDisplayingData = false;
	m_Scene = NULL;


	// setup
	ui.setupUi(this);

	ui.AmbientColor->SetAlphaChannel(false);
	connect(ui.AmbientColor, SIGNAL(ColorChanged(QColor)), this, SLOT(OnAmbientColorChanged()));

	ui.FogType->addItem(tr("No fog"), QVariant(Scene3DBase::FOG_NONE));
	ui.FogType->addItem(tr("Linear"), QVariant(Scene3DBase::FOG_LINEAR));
	ui.FogType->addItem(tr("Exponential"), QVariant(Scene3DBase::FOG_EXP));
	ui.FogType->addItem(tr("Exponential squared"), QVariant(Scene3DBase::FOG_EXP2));	
	connect(ui.FogType, SIGNAL(currentIndexChanged(int)), this, SLOT(OnFogTypeChanged()));

	ui.FogColor->SetAlphaChannel(false);
	connect(ui.FogColor, SIGNAL(ColorChanged(QColor)), this, SLOT(OnFogColorChanged()));

	ui.FogDensity->setMinimum(0);
	ui.FogDensity->setMaximum(1);
	ui.FogDensity->setDecimals(4);
	ui.FogDensity->setSingleStep(0.0001f);
	ui.FogDensity->setKeyboardTracking(false);
	connect(ui.FogDensity, SIGNAL(valueChanged(double)), this, SLOT(OnFogDensityChanged()));

	ui.FogStart->setMinimum(0);
	ui.FogStart->setMaximum(1000000.0f);
	ui.FogStart->setDecimals(2);
	ui.FogStart->setKeyboardTracking(false);
	connect(ui.FogStart, SIGNAL(valueChanged(double)), this, SLOT(OnFogStartChanged()));

	ui.FogEnd->setMinimum(0);
	ui.FogEnd->setMaximum(1000000.0f);
	ui.FogEnd->setDecimals(2);
	ui.FogEnd->setKeyboardTracking(false);
	connect(ui.FogEnd, SIGNAL(valueChanged(double)), this, SLOT(OnFogEndChanged()));
}

//////////////////////////////////////////////////////////////////////////
PropScene3D::~PropScene3D()
{

}

//////////////////////////////////////////////////////////////////////////
void PropScene3D::SetScene(Scene3DBase* scene)
{
	m_Scene = scene;
	DisplayData();
}

//////////////////////////////////////////////////////////////////////////
void PropScene3D::DisplayData()
{
	if (!m_Scene) return;

	m_IsDisplayingData = true;

	ui.AmbientColor->SetColor(QtUtil::OgreColorToQColor(m_Scene->GetAmbientLightColor()));

	int index = ui.FogType->findData(QVariant(m_Scene->GetFogType()));
	if (index < 0) index = 0;
	ui.FogType->setCurrentIndex(index);

	ui.FogColor->SetColor(QtUtil::OgreColorToQColor(m_Scene->GetFogColor()));
	ui.FogDensity->setValue(m_Scene->GetFogDensity());
	ui.FogStart->setValue(m_Scene->GetFogStart());
	ui.FogEnd->setValue(m_Scene->GetFogEnd());

	SetState();

	m_IsDisplayingData = false;
}

//////////////////////////////////////////////////////////////////////////
void PropScene3D::OnAmbientColorChanged()
{
	if (m_IsDisplayingData) return;
	if (!m_Scene) return;

	m_Scene->SetAmbientLightColor(QtUtil::QColorToOgreColor(ui.AmbientColor->GetColor()));
}

//////////////////////////////////////////////////////////////////////////
void PropScene3D::OnFogTypeChanged()
{
	if (m_IsDisplayingData) return;
	if (!m_Scene) return;

	Scene3DBase::FogType fogType = static_cast<Scene3DBase::FogType>(ui.FogType->itemData(ui.FogType->currentIndex()).toInt());
	m_Scene->SetFogType(fogType);
	SetState();
}

//////////////////////////////////////////////////////////////////////////
void PropScene3D::OnFogColorChanged()
{
	if (m_IsDisplayingData) return;
	if (!m_Scene) return;

	m_Scene->SetFogColor(QtUtil::QColorToOgreColor(ui.FogColor->GetColor()));
}

//////////////////////////////////////////////////////////////////////////
void PropScene3D::OnFogDensityChanged()
{
	if (m_IsDisplayingData) return;
	if (!m_Scene) return;

	m_Scene->SetFogDensity(ui.FogDensity->value());
}

//////////////////////////////////////////////////////////////////////////
void PropScene3D::OnFogStartChanged()
{
	if (m_IsDisplayingData) return;
	if (!m_Scene) return;

	m_Scene->SetFogStart(ui.FogStart->value());
}

//////////////////////////////////////////////////////////////////////////
void PropScene3D::OnFogEndChanged()
{
	if (m_IsDisplayingData) return;
	if (!m_Scene) return;

	m_Scene->SetFogEnd(ui.FogEnd->value());
}

//////////////////////////////////////////////////////////////////////////
void PropScene3D::SetState()
{
	if (!m_Scene) return;

	bool linearFog = m_Scene->GetFogType() == Scene3DBase::FOG_LINEAR;
	bool expFog = m_Scene->GetFogType() == Scene3DBase::FOG_EXP || m_Scene->GetFogType() == Scene3DBase::FOG_EXP2;

	ui.LabelFogColor->setVisible(linearFog || expFog);
	ui.FogColor->setVisible(linearFog || expFog);

	ui.LabelFogDensity->setVisible(expFog);
	ui.FogDensity->setVisible(expFog);

	ui.LabelFogStart->setVisible(linearFog);
	ui.FogStart->setVisible(linearFog);
	ui.LabelFogEnd->setVisible(linearFog);
	ui.FogEnd->setVisible(linearFog);
}


} // namespace Armed
