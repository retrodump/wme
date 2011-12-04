// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "CanCloseDialog.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
CanCloseDialog::CanCloseDialog(QWidget* parent) : QDialog(parent)
{
	m_Ui.setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	m_Save = false;

	connect(m_Ui.YesButton, SIGNAL(clicked()), this, SLOT(OnYes()));
	connect(m_Ui.NoButton, SIGNAL(clicked()), this, SLOT(OnNo()));
	connect(m_Ui.CancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

//////////////////////////////////////////////////////////////////////////
CanCloseDialog::~CanCloseDialog()
{

}

//////////////////////////////////////////////////////////////////////////
void CanCloseDialog::AddFile(const QString& fileName)
{
	m_Ui.FileList->addItem(fileName);
}

//////////////////////////////////////////////////////////////////////////
void CanCloseDialog::OnYes()
{
	m_Save = true;
	accept();
}

//////////////////////////////////////////////////////////////////////////
void CanCloseDialog::OnNo()
{
	m_Save = false;
	accept();
}


} // namespace Armed
