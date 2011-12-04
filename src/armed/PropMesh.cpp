// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "PropMesh.h"
#include "MeshEntity.h"
#include "QtUtil.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
PropMesh::PropMesh(QWidget* parent) : PropSectionBase(parent)
{
	m_Entity = NULL;
	m_IsDisplayingData = false;


	// setup
	ui.setupUi(this);

	ui.MeshFile->SetAllowedTypes("mesh");
	ui.MeshFile->SetPlaceholderText(tr("Choose mesh"));
	connect(ui.MeshFile, SIGNAL(FileChanged(QString)), this, SLOT(OnMeshChanged()));

	ui.SkeletonFile->SetAllowedTypes("skeleton");
	ui.SkeletonFile->SetPlaceholderText(tr("Choose skeleton"));
	connect(ui.SkeletonFile, SIGNAL(FileChanged(QString)), this, SLOT(OnSkeletonChanged()));

	connect(ui.DelSkeleton, SIGNAL(clicked()), this, SLOT(OnRemoveSkeleton()));

	ui.DelSkeleton->setIcon(QIcon(":/icons/cross.png"));
}

//////////////////////////////////////////////////////////////////////////
PropMesh::~PropMesh()
{

}

//////////////////////////////////////////////////////////////////////////
void PropMesh::DisplayData()
{
	if (!m_Entity) return;

	m_IsDisplayingData = true;

	ui.MeshFile->SetFileName(QtUtil::WideToQString(m_Entity->GetMeshFileName()));
	ui.SkeletonFile->SetFileName(QtUtil::WideToQString(m_Entity->GetSkeletonFileName()));

	SetState();

	m_IsDisplayingData = false;
}

//////////////////////////////////////////////////////////////////////////
void PropMesh::SetEntity(MeshEntity* entity)
{
	m_Entity = entity;
	DisplayData();
}

//////////////////////////////////////////////////////////////////////////
void PropMesh::OnMeshChanged()
{
	if (m_IsDisplayingData) return;
	if (!m_Entity) return;

	m_Entity->SetMesh(QtUtil::QStringToWide(ui.MeshFile->GetFileName()), true);
	DisplayData();

	emit MeshChanged();
}

//////////////////////////////////////////////////////////////////////////
void PropMesh::OnSkeletonChanged()
{
	if (m_IsDisplayingData) return;
	if (!m_Entity) return;

	m_Entity->SetSkeleton(QtUtil::QStringToWide(ui.SkeletonFile->GetFileName()), true);
	DisplayData();

	emit SkeletonChanged();
}

//////////////////////////////////////////////////////////////////////////
void PropMesh::OnRemoveSkeleton()
{
	if (m_IsDisplayingData) return;
	if (!m_Entity) return;

	m_Entity->RemoveSkeleton(true);
	DisplayData();
}

//////////////////////////////////////////////////////////////////////////
void PropMesh::SetState()
{
	ui.DelSkeleton->setDisabled(ui.SkeletonFile->GetFileName().isEmpty());

	ui.SkeletonFile->setEnabled(m_Entity->SupportsSkeletalAnimation());
}


} // namespace Armed
