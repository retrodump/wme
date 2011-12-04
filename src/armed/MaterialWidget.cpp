// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "MaterialWidget.h"
#include "MaterialInstance.h"
#include "Material.h"
#include "Game.h"
#include "Renderer.h"
#include "MaterialManager.h"
#include "GenericProperty.h"
#include "StringUtil.h"
#include "QtUtil.h"
#include "PropertyColor.h"
#include "PropertyColorWidget.h"
#include "PropertyFloat.h"
#include "PropertyFloatWidget.h"
#include "PropertyTexture.h"
#include "PropertyTextureWidget.h"
#include "FileButton.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
MaterialWidget::MaterialWidget(QWidget* parent) : QWidget(parent)
{
	m_Material = NULL;

	m_IgnoreEvents = false;

	m_MatButton = new FileButton(this);
	m_MatButton->SetAllowedTypes("material");
	m_MatButton->SetPlaceholderText(tr("Choose material"));
	connect(m_MatButton, SIGNAL(FileChanged(QString)), this, SLOT(OnMaterialChanged(QString)));

	m_DelButton = new QPushButton();
	m_DelButton->setFixedWidth(23);
	m_DelButton->setFixedHeight(23);
	m_DelButton->setIcon(QIcon(":/icons/cross.png"));
	m_DelButton->setToolTip(tr("Remove material"));
	connect(m_DelButton, SIGNAL(clicked()), this, SLOT(OnMaterialRemoved()));

	QHBoxLayout* topLayout = new QHBoxLayout();
	topLayout->setMargin(0);	
	topLayout->setSpacing(0);	
	topLayout->addWidget(m_MatButton);
	topLayout->addWidget(m_DelButton);

	m_Layout = new QVBoxLayout(this);
	m_Layout->setMargin(0);	
	m_Layout->addLayout(topLayout);

	QFrame* line = new QFrame(this);
	line->setFrameShape(QFrame::HLine);
	line->setFrameShadow(QFrame::Sunken);
	m_Layout->addWidget(line);


	m_FormLayout = new QFormLayout;
	m_FormLayout->setHorizontalSpacing(4);
	m_FormLayout->setVerticalSpacing(2);

	m_Layout->addLayout(m_FormLayout);
}

//////////////////////////////////////////////////////////////////////////
MaterialWidget::~MaterialWidget()
{
}

//////////////////////////////////////////////////////////////////////////
void MaterialWidget::OnMaterialChanged(const QString& fileName)
{
	if (m_IgnoreEvents) return;

	ResetMaterial(fileName);
	Rebuild();
}

//////////////////////////////////////////////////////////////////////////
void MaterialWidget::OnMaterialRemoved()
{
	ResetMaterial("");
	Rebuild();

	// removing material sometimes triggers ogre bug, is there a better place for this call?
	Game::GetInstance()->GetRenderer()->ScheduleClearSceneManagerPassMaps();
}

//////////////////////////////////////////////////////////////////////////
void MaterialWidget::OnMaterialPropChanged()
{
	m_Material->SetDirty(true);
	ReApplyMaterial();
}

//////////////////////////////////////////////////////////////////////////
void MaterialWidget::OnMaterialDefChanged(const QString& fileName)
{
	m_Material->SetMaterialDef(QtUtil::QStringToWide(fileName));
	ReApplyMaterial();
	Rebuild();	
}

//////////////////////////////////////////////////////////////////////////
void MaterialWidget::Rebuild()
{
	// currently it doesn't seem to be possible to reliably change QFormLayout's content
	// so we need to delete and recreate everything
	emit RebuildNeeded();
}

//////////////////////////////////////////////////////////////////////////
void MaterialWidget::BuildPropGui()
{
	Material* matDef = m_Material->GetMaterialDef();
	if (!matDef) return;


	FileButton* matDefButton = new FileButton(this, QtUtil::WideToQString(matDef->GetFileName()));
	matDefButton->SetAllowedTypes("matdef");
	connect(matDefButton, SIGNAL(FileChanged(QString)), this, SLOT(OnMaterialDefChanged(QString)));
	m_FormLayout->addRow(tr("Type:"), matDefButton);


	const GenericPropertyList& props = matDef->GetProperties();
	foreach (GenericProperty* prop, props)
	{
		QWidget* widg = GetPropWidget(prop);
		if (widg) m_FormLayout->addRow(QtUtil::WideToQString(prop->GetLabel()) + ":", widg);
	}	
}

//////////////////////////////////////////////////////////////////////////
QWidget* MaterialWidget::GetPropWidget(GenericProperty* prop)
{
	GenericPropertyValue* val = m_Material->GetPropertyValue(prop->GetName());
	if (!val) return NULL;


	if (prop->GetType() == L"color")
	{
		PropertyColorWidget* widg = new PropertyColorWidget(this, static_cast<PropertyValueColor*>(val));
		connect(widg, SIGNAL(PropertyChanged()), this, SLOT(OnMaterialPropChanged()));
		return widg;
	}
	else if (prop->GetType() == L"float")
	{
		PropertyFloatWidget* widg = new PropertyFloatWidget(this, static_cast<PropertyValueFloat*>(val));
		connect(widg, SIGNAL(PropertyChanged()), this, SLOT(OnMaterialPropChanged()));
		return widg;
	}
	if (prop->GetType() == L"texture")
	{
		PropertyTextureWidget* widg = new PropertyTextureWidget(this, static_cast<PropertyValueTexture*>(val));
		connect(widg, SIGNAL(PropertyChanged()), this, SLOT(OnMaterialPropChanged()));
		return widg;
	}
	else return new QPushButton;
}

//////////////////////////////////////////////////////////////////////////
void MaterialWidget::SetState()
{
	m_DelButton->setEnabled(m_Material != NULL);
}


} // namespace Armed
