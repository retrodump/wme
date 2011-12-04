// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "PropLight.h"
#include "LightEntity.h"
#include "QtUtil.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
PropLight::PropLight(QWidget* parent) : PropSectionBase(parent)
{
	m_Light = NULL;
	m_IsDisplayingData = false;

	// setup
	ui.setupUi(this);

	ui.LightType->insertItem(0, tr("Directional"), QVariant(LightEntity::LIGHT_DIRECTIONAL));
	ui.LightType->insertItem(1, tr("Point"), QVariant(LightEntity::LIGHT_POINT));
	ui.LightType->insertItem(2, tr("Spot"), QVariant(LightEntity::LIGHT_SPOT));
	connect(ui.LightType, SIGNAL(currentIndexChanged(int)), this, SLOT(OnLightTypeChanged()));

	ui.DiffuseColor->SetAlphaChannel(false);
	connect(ui.DiffuseColor, SIGNAL(ColorChanged(QColor)), this, SLOT(OnDiffuseColorChanged()));

	ui.SpecularColor->SetAlphaChannel(false);
	connect(ui.SpecularColor, SIGNAL(ColorChanged(QColor)), this, SLOT(OnSpecularColorChanged()));

	connect(ui.CastShadows, SIGNAL(stateChanged(int)), this, SLOT(OnCastShadowsChanged()));

	ui.Range->setMinimum(0);
	ui.Range->setMaximum(std::numeric_limits<Ogre::Real>::max());
	ui.Range->setDecimals(2);
	ui.Range->setKeyboardTracking(false);
	connect(ui.Range, SIGNAL(valueChanged(double)), this, SLOT(OnAttenuationChanged()));

	ui.AttnConstant->setMinimum(0);
	ui.AttnConstant->setMaximum(std::numeric_limits<Ogre::Real>::max());
	ui.AttnConstant->setDecimals(2);
	ui.AttnConstant->setSingleStep(0.1f);
	ui.AttnConstant->setKeyboardTracking(false);
	connect(ui.AttnConstant, SIGNAL(valueChanged(double)), this, SLOT(OnAttenuationChanged()));

	ui.AttnLinear->setMinimum(0);
	ui.AttnLinear->setMaximum(std::numeric_limits<Ogre::Real>::max());
	ui.AttnLinear->setDecimals(2);
	ui.AttnLinear->setSingleStep(0.1f);
	ui.AttnLinear->setKeyboardTracking(false);
	connect(ui.AttnLinear, SIGNAL(valueChanged(double)), this, SLOT(OnAttenuationChanged()));

	ui.AttnQuadratic->setMinimum(0);
	ui.AttnQuadratic->setMaximum(std::numeric_limits<Ogre::Real>::max());
	ui.AttnQuadratic->setDecimals(2);
	ui.AttnQuadratic->setSingleStep(0.1f);
	ui.AttnQuadratic->setKeyboardTracking(false);
	connect(ui.AttnQuadratic, SIGNAL(valueChanged(double)), this, SLOT(OnAttenuationChanged()));

	ui.InnerAngle->setMinimum(1);
	ui.InnerAngle->setMaximum(179);
	ui.InnerAngle->setDecimals(2);
	ui.InnerAngle->setKeyboardTracking(false);
	connect(ui.InnerAngle, SIGNAL(valueChanged(double)), this, SLOT(OnSpotlightChanged()));

	ui.OuterAngle->setMinimum(1);
	ui.OuterAngle->setMaximum(179);
	ui.OuterAngle->setDecimals(2);
	ui.OuterAngle->setKeyboardTracking(false);
	connect(ui.OuterAngle, SIGNAL(valueChanged(double)), this, SLOT(OnSpotlightChanged()));

	ui.Falloff->setMinimum(-std::numeric_limits<Ogre::Real>::max());
	ui.Falloff->setMaximum(std::numeric_limits<Ogre::Real>::max());
	ui.Falloff->setDecimals(2);
	ui.Falloff->setSingleStep(0.1f);
	ui.Falloff->setKeyboardTracking(false);
	connect(ui.Falloff, SIGNAL(valueChanged(double)), this, SLOT(OnSpotlightChanged()));
}

//////////////////////////////////////////////////////////////////////////
PropLight::~PropLight()
{

}

//////////////////////////////////////////////////////////////////////////
void PropLight::DisplayData()
{
	if (!m_Light) return;
	m_IsDisplayingData = true;

	int index = ui.LightType->findData(QVariant(m_Light->GetLightType()));
	if (index < 0) index = 0;
	ui.LightType->setCurrentIndex(index);

	ui.DiffuseColor->SetColor(QtUtil::OgreColorToQColor(m_Light->GetDiffuseColor()));
	ui.SpecularColor->SetColor(QtUtil::OgreColorToQColor(m_Light->GetSpecularColor()));
	ui.CastShadows->setCheckState(m_Light->GetCastShadows() ? Qt::Checked : Qt::Unchecked);
	ui.Range->setValue(m_Light->GetAttenuationRange());
	ui.AttnConstant->setValue(m_Light->GetAttenuationConstant());
	ui.AttnLinear->setValue(m_Light->GetAttenuationLinear());
	ui.AttnQuadratic->setValue(m_Light->GetAttenuationQuadratic());
	ui.InnerAngle->setValue(m_Light->GetSpotlightInnerAngle().valueDegrees());
	ui.OuterAngle->setValue(m_Light->GetSpotlightOuterAngle().valueDegrees());
	ui.Falloff->setValue(m_Light->GetSpotlightFalloff());

	SetState();

	m_IsDisplayingData = false;
}

//////////////////////////////////////////////////////////////////////////
void PropLight::SetLight(LightEntity* light)
{
	m_Light = light;
	DisplayData();
}

//////////////////////////////////////////////////////////////////////////
void PropLight::OnLightTypeChanged()
{
	if (m_IsDisplayingData) return;
	if (!m_Light) return;

	LightEntity::LightType lightType = static_cast<LightEntity::LightType>(ui.LightType->itemData(ui.LightType->currentIndex()).toInt());
	m_Light->SetLightType(lightType);
	SetState();
}

//////////////////////////////////////////////////////////////////////////
void PropLight::OnDiffuseColorChanged()
{
	if (m_IsDisplayingData) return;
	if (!m_Light) return;

	m_Light->SetDiffuseColor(QtUtil::QColorToOgreColor(ui.DiffuseColor->GetColor()));
}

//////////////////////////////////////////////////////////////////////////
void PropLight::OnSpecularColorChanged()
{
	if (m_IsDisplayingData) return;
	if (!m_Light) return;

	m_Light->SetSpecularColor(QtUtil::QColorToOgreColor(ui.SpecularColor->GetColor()));
}

//////////////////////////////////////////////////////////////////////////
void PropLight::OnAttenuationChanged()
{
	if (m_IsDisplayingData) return;
	if (!m_Light) return;

	m_Light->SetAttenuation(ui.Range->value(), ui.AttnConstant->value(), ui.AttnLinear->value(), ui.AttnQuadratic->value());
}

//////////////////////////////////////////////////////////////////////////
void PropLight::OnSpotlightChanged()
{
	if (m_IsDisplayingData) return;
	if (!m_Light) return;

	m_Light->SetSpotlightRange(Ogre::Radian(Ogre::Degree(ui.InnerAngle->value())), Ogre::Radian(Ogre::Degree(ui.OuterAngle->value())), ui.Falloff->value());
}

//////////////////////////////////////////////////////////////////////////
void PropLight::OnCastShadowsChanged()
{
	if (m_IsDisplayingData) return;
	if (!m_Light) return;

	m_Light->SetCastShadows(ui.CastShadows->checkState() == Qt::Checked);
}

//////////////////////////////////////////////////////////////////////////
void PropLight::SetState()
{
	if (!m_Light) return;

	bool spot = m_Light->GetLightType() == LightEntity::LIGHT_SPOT;

	ui.LineSpot->setVisible(spot);
	ui.LabelSpot->setVisible(spot);
	ui.LabelInner->setVisible(spot);
	ui.InnerAngle->setVisible(spot);
	ui.LabelOuter->setVisible(spot);
	ui.OuterAngle->setVisible(spot);
	ui.LabelFalloff->setVisible(spot);
	ui.Falloff->setVisible(spot);

	bool attn = m_Light->GetLightType() == LightEntity::LIGHT_SPOT | m_Light->GetLightType() == LightEntity::LIGHT_POINT;

	ui.LineAttn->setVisible(attn);
	ui.LabelAttn->setVisible(attn);
	ui.LabelRange->setVisible(attn);
	ui.Range->setVisible(attn);
	ui.LabelConstant->setVisible(attn);
	ui.AttnConstant->setVisible(attn);
	ui.LabelLinear->setVisible(attn);
	ui.AttnLinear->setVisible(attn);
	ui.LabelQuadratic->setVisible(attn);
	ui.AttnQuadratic->setVisible(attn);
}


} // namespace Armed
