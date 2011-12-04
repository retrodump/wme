// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "PropCamera.h"
#include "CameraEntity.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
PropCamera::PropCamera(QWidget* parent) : PropSectionBase(parent)
{
	m_Camera = NULL;
	m_IsDisplayingData = false;


	// setup
	ui.setupUi(this);

	ui.ProjectionType->insertItem(0, tr("Perspective"), QVariant(CameraEntity::PROJECTION_PERSPECTIVE));
	ui.ProjectionType->insertItem(1, tr("Orthographic"), QVariant(CameraEntity::PROJECTION_ORTHOGRAPHIC));
	connect(ui.ProjectionType, SIGNAL(currentIndexChanged(int)), this, SLOT(OnProjectionTypeChanged()));

	ui.OrthoHeight->setMinimum(-std::numeric_limits<Ogre::Real>::max());
	ui.OrthoHeight->setMaximum(std::numeric_limits<Ogre::Real>::max());
	ui.OrthoHeight->setDecimals(2);
	ui.OrthoHeight->setKeyboardTracking(false);
	connect(ui.OrthoHeight, SIGNAL(valueChanged(double)), this, SLOT(OnOrthoHeightChanged()));

	ui.Fov->setMinimum(1);
	ui.Fov->setMaximum(179);
	ui.Fov->setDecimals(2);
	ui.Fov->setKeyboardTracking(false);
	connect(ui.Fov, SIGNAL(valueChanged(double)), this, SLOT(OnFovChanged()));

	ui.FovSlider->setMinimum(ui.Fov->minimum() * 100.0f);
	ui.FovSlider->setMaximum(ui.Fov->maximum() * 100.0f);
	ui.FovSlider->setSingleStep(10);
	connect(ui.FovSlider, SIGNAL(valueChanged(int)), this, SLOT(OnFovSliderChanged()));

	ui.ClipNear->setMinimum(0.01f);
	ui.ClipNear->setMaximum(10000000.0f);
	ui.ClipNear->setDecimals(2);
	ui.ClipNear->setKeyboardTracking(false);
	connect(ui.ClipNear, SIGNAL(valueChanged(double)), this, SLOT(OnClipNearChanged()));

	ui.ClipFar->setMinimum(0.01f);
	ui.ClipFar->setMaximum(10000000.0f);
	ui.ClipFar->setDecimals(2);
	ui.ClipFar->setKeyboardTracking(false);
	connect(ui.ClipFar, SIGNAL(valueChanged(double)), this, SLOT(OnClipFarChanged()));
}

//////////////////////////////////////////////////////////////////////////
PropCamera::~PropCamera()
{

}

//////////////////////////////////////////////////////////////////////////
void PropCamera::DisplayData()
{
	if (!m_Camera) return;

	m_IsDisplayingData = true;

	int index = ui.ProjectionType->findData(QVariant(m_Camera->GetProjectionType()));
	if (index < 0) index = 0;
	ui.ProjectionType->setCurrentIndex(index);

	ui.Fov->setValue(m_Camera->GetFOVy().valueDegrees());
	ui.FovSlider->setValue(m_Camera->GetFOVy().valueDegrees() * 100);

	ui.OrthoHeight->setValue(m_Camera->GetOrthoWindowHeight());

	ui.ClipNear->setValue(m_Camera->GetNearClipDistance());
	ui.ClipFar->setValue(m_Camera->GetFarClipDistance());

	SetState();

	m_IsDisplayingData = false;
}

//////////////////////////////////////////////////////////////////////////
void PropCamera::SetCamera(CameraEntity* camera)
{
	m_Camera = camera;
	DisplayData();
}

//////////////////////////////////////////////////////////////////////////
void PropCamera::OnProjectionTypeChanged()
{
	if (m_IsDisplayingData) return;
	if (!m_Camera) return;

	CameraEntity::ProjectionType projType = static_cast<CameraEntity::ProjectionType>(ui.ProjectionType->itemData(ui.ProjectionType->currentIndex()).toInt());
	m_Camera->SetProjectionType(projType);
	SetState();
}

//////////////////////////////////////////////////////////////////////////
void PropCamera::OnOrthoHeightChanged()
{
	if (m_IsDisplayingData) return;
	if (!m_Camera) return;

	m_Camera->SetOrthoWindowHeight(ui.OrthoHeight->value());
}

//////////////////////////////////////////////////////////////////////////
void PropCamera::OnFovChanged()
{
	if (m_IsDisplayingData) return;
	if (!m_Camera) return;

	m_Camera->SetFOVy(Ogre::Radian(Ogre::Degree(ui.Fov->value())));
	ui.FovSlider->setValue(ui.Fov->value() * 100);
}

//////////////////////////////////////////////////////////////////////////
void PropCamera::OnFovSliderChanged()
{
	if (m_IsDisplayingData) return;
	if (!m_Camera) return;

	double val = (double)ui.FovSlider->value() / 100.0f;
	ui.Fov->setValue(val);
}

//////////////////////////////////////////////////////////////////////////
void PropCamera::OnClipNearChanged()
{
	if (m_IsDisplayingData) return;
	if (!m_Camera) return;

	m_Camera->SetNearClipDistance(ui.ClipNear->value());
}

//////////////////////////////////////////////////////////////////////////
void PropCamera::OnClipFarChanged()
{
	if (m_IsDisplayingData) return;
	if (!m_Camera) return;

	m_Camera->SetFarClipDistance(ui.ClipFar->value());
}

//////////////////////////////////////////////////////////////////////////
void PropCamera::SetState()
{
	if (!m_Camera) return;

	bool ortho = m_Camera->GetProjectionType() == CameraEntity::PROJECTION_ORTHOGRAPHIC;

	ui.LabelFov->setVisible(!ortho);
	ui.Fov->setVisible(!ortho);
	ui.FovSlider->setVisible(!ortho);

	ui.LabelOrtho->setVisible(ortho);
	ui.OrthoHeight->setVisible(ortho);
}


} // namespace Armed
