// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "PropTransform.h"
#include "NodeSelection.h"
#include "SceneNodeEditor.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
PropTransform::PropTransform(QWidget* parent) : PropSectionBase(parent)
{
	m_Selection = NULL;
	m_IsDisplayingData = false;

	// setup
	ui.setupUi(this);

	SetupSpinBox(ui.PosX);
	SetupSpinBox(ui.PosY);
	SetupSpinBox(ui.PosZ);
	connect(ui.PosX, SIGNAL(valueChanged(double)), this, SLOT(OnPosChanged()));
	connect(ui.PosY, SIGNAL(valueChanged(double)), this, SLOT(OnPosChanged()));
	connect(ui.PosZ, SIGNAL(valueChanged(double)), this, SLOT(OnPosChanged()));

	SetupSpinBox(ui.RotX);
	SetupSpinBox(ui.RotY);
	SetupSpinBox(ui.RotZ);
	connect(ui.RotX, SIGNAL(valueChanged(double)), this, SLOT(OnRotChanged()));
	connect(ui.RotY, SIGNAL(valueChanged(double)), this, SLOT(OnRotChanged()));
	connect(ui.RotZ, SIGNAL(valueChanged(double)), this, SLOT(OnRotChanged()));

	SetupSpinBox(ui.ScaleX);
	SetupSpinBox(ui.ScaleY);
	SetupSpinBox(ui.ScaleZ);
	connect(ui.ScaleX, SIGNAL(valueChanged(double)), this, SLOT(OnScaleChanged()));
	connect(ui.ScaleY, SIGNAL(valueChanged(double)), this, SLOT(OnScaleChanged()));
	connect(ui.ScaleZ, SIGNAL(valueChanged(double)), this, SLOT(OnScaleChanged()));
}

//////////////////////////////////////////////////////////////////////////
PropTransform::~PropTransform()
{

}

//////////////////////////////////////////////////////////////////////////
void PropTransform::SetupSpinBox(QDoubleSpinBox* spinBox)
{
	spinBox->setMinimum(-std::numeric_limits<Ogre::Real>::max());
	spinBox->setMaximum(std::numeric_limits<Ogre::Real>::max());
	spinBox->setDecimals(3);

	spinBox->setKeyboardTracking(false);
}

//////////////////////////////////////////////////////////////////////////
void PropTransform::SetSelection(NodeSelection* selection)
{
	m_Selection = selection;
	DisplayData();
}

//////////////////////////////////////////////////////////////////////////
void PropTransform::DisplayData()
{
	if (!m_Selection) return;

	m_IsDisplayingData = true;

	// display position
	Ogre::Vector3 pos = m_Selection->GetOrigin();
	ui.PosX->setValue(pos.x);
	ui.PosY->setValue(pos.y);
	ui.PosZ->setValue(pos.z);

	ui.PosX->setEnabled(m_Selection->IsMovable());
	ui.PosY->setEnabled(m_Selection->IsMovable());
	ui.PosZ->setEnabled(m_Selection->IsMovable());


	// display orientation
	Ogre::Quaternion rot = m_Selection->GetOrientation();

	Ogre::Matrix3 rotMat;
	Ogre::Radian yaw, pitch, roll;
	rot.ToRotationMatrix(rotMat);
	rotMat.ToEulerAnglesYXZ(yaw, pitch, roll);

	ui.RotX->setValue(FixAngle(pitch.valueDegrees()));
	ui.RotY->setValue(FixAngle(yaw.valueDegrees()));
	ui.RotZ->setValue(FixAngle(roll.valueDegrees()));

	ui.RotX->setEnabled(m_Selection->IsRotatable());
	ui.RotY->setEnabled(m_Selection->IsRotatable());
	ui.RotZ->setEnabled(m_Selection->IsRotatable());


	// display scale
	Ogre::Vector3 scale = m_Selection->GetScale();
	ui.ScaleX->setValue(scale.x);
	ui.ScaleY->setValue(scale.y);
	ui.ScaleZ->setValue(scale.z);

	ui.ScaleX->setEnabled(m_Selection->IsScalable());
	ui.ScaleY->setEnabled(m_Selection->IsScalable());
	ui.ScaleZ->setEnabled(m_Selection->IsScalable());


	m_IsDisplayingData = false;
}

//////////////////////////////////////////////////////////////////////////
void PropTransform::OnPosChanged()
{
	if (m_IsDisplayingData) return;

	Ogre::Vector3 oldPos = m_Selection->GetOrigin();
	Ogre::Vector3 newPos(ui.PosX->value(), ui.PosY->value(), ui.PosZ->value());

	m_Selection->MoveBy(newPos - oldPos, false, false);
	m_Selection->GetEditor()->UpdateGizmos();	
}

//////////////////////////////////////////////////////////////////////////
void PropTransform::OnRotChanged()
{
	if (m_IsDisplayingData) return;

	Ogre::Quaternion oldRot = m_Selection->GetOrientation();

	Ogre::Radian yaw, pitch, roll;
	yaw = Ogre::Degree(ui.RotY->value());
	pitch = Ogre::Degree(ui.RotX->value());
	roll = Ogre::Degree(ui.RotZ->value());

	Ogre::Matrix3 rotMat;
	rotMat.FromEulerAnglesYXZ(yaw, pitch, roll);

	m_Selection->SetOrientation(Ogre::Quaternion(rotMat), false, false);
	m_Selection->GetEditor()->UpdateGizmos();
}

//////////////////////////////////////////////////////////////////////////
void PropTransform::OnScaleChanged()
{
	if (m_IsDisplayingData) return;

	Ogre::Vector3 newScale(ui.ScaleX->value(), ui.ScaleY->value(), ui.ScaleZ->value());

	// bad things happen with zero scale
	if (newScale.x == 0.0f) newScale.x = 0.000001f;
	if (newScale.y == 0.0f) newScale.y = 0.000001f;
	if (newScale.z == 0.0f) newScale.z = 0.000001f;

	m_Selection->GetAnchorNode()->setScale(newScale);
	m_Selection->GetEditor()->UpdateGizmos();
}

//////////////////////////////////////////////////////////////////////////
Ogre::Real PropTransform::FixAngle(Ogre::Real angle)
{
	if (angle < 0.0f) angle += 360.0f;
	if (angle >= 360.0f) angle -= 360.0f;
	return angle;
}

} // namespace Armed
