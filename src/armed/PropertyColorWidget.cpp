// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "PropertyColorWidget.h"
#include "PropertyColor.h"
#include "ColorButton.h"
#include "QtUtil.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
PropertyColorWidget::PropertyColorWidget(QWidget* parent, PropertyValueColor* value) : GenericPropertyWidget(parent)
{
	m_Value = value;

	m_ColorButton = new ColorButton(this, QtUtil::OgreColorToQColor(value->GetColor()));
	m_ColorButton->SetAlphaChannel(value->GetParentProp()->GetIncludeAlpha());

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(m_ColorButton);

	connect(m_ColorButton, SIGNAL(ColorChanged(QColor)), this, SLOT(OnColorChanged(QColor)));
}

//////////////////////////////////////////////////////////////////////////
PropertyColorWidget::~PropertyColorWidget()
{

}

//////////////////////////////////////////////////////////////////////////
void PropertyColorWidget::OnColorChanged(const QColor& color)
{
	m_Value->SetColor(QtUtil::QColorToOgreColor(color));
	emit PropertyChanged();
}


} // namespace Armed
