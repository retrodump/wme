// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "PropertyTextureWidget.h"
#include "PropertyTexture.h"
#include "TextureButton.h"
#include "QtUtil.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
PropertyTextureWidget::PropertyTextureWidget(QWidget* parent, PropertyValueTexture* value) : GenericPropertyWidget(parent)
{
	m_Value = value;

	m_TextureButton = new TextureButton(this);
	m_TextureButton->SetImage(QtUtil::WideToQString(value->GetFileName()));


	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(m_TextureButton);

	connect(m_TextureButton, SIGNAL(TextureChanged(QString)), this, SLOT(OnTextureChanged(QString)));
}

//////////////////////////////////////////////////////////////////////////
PropertyTextureWidget::~PropertyTextureWidget()
{

}

//////////////////////////////////////////////////////////////////////////
void PropertyTextureWidget::OnTextureChanged(const QString& fileName)
{
	m_Value->SetFileName(QtUtil::QStringToWide(fileName));
	emit PropertyChanged();
}


} // namespace Armed
