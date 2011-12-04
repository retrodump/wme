// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "FilePickerDialog.h"
#include "QtUtil.h"
#include "Project.h"
#include "QtUtil.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
FilePickerDialog::FilePickerDialog(QWidget* parent) : QDialog(parent)
{
	m_Ui.setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	connect(m_Ui.OkButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(m_Ui.CancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(this, SIGNAL(finished(int)), this, SLOT(OnFinished()));

	connect(m_Ui.ProjectTree, SIGNAL(PathSelected(QString)), this, SLOT(OnPathSelected(QString)));
	connect(m_Ui.ProjectTree, SIGNAL(doubleClicked(QModelIndex)), m_Ui.OkButton, SLOT(click()));

	connect(m_Ui.FileName, SIGNAL(textChanged(QString)), this, SLOT(OnPathEdited(QString)));

	OnPathEdited("");

	QSettings settings;
	settings.beginGroup("FilePickerDialog");
	restoreGeometry(settings.value("Geometry").toByteArray());
	m_Ui.MainSplitter->restoreState(settings.value("MainSplitter").toByteArray());
	settings.endGroup();	
}

//////////////////////////////////////////////////////////////////////////
FilePickerDialog::~FilePickerDialog()
{

}

//////////////////////////////////////////////////////////////////////////
void FilePickerDialog::OnFinished()
{
	QSettings settings;
	settings.beginGroup("FilePickerDialog");
	settings.setValue("Geometry", saveGeometry());
	settings.setValue("MainSplitter", m_Ui.MainSplitter->saveState());
	settings.endGroup();
}

//////////////////////////////////////////////////////////////////////////
void FilePickerDialog::OnPathSelected(const QString& path)
{
	if (QtUtil::NormalizeFileName(path) != QtUtil::NormalizeFileName(m_Ui.FileName->text()))
		m_Ui.FileName->setText(QDir::toNativeSeparators(path));
}

//////////////////////////////////////////////////////////////////////////
void FilePickerDialog::OnPathEdited(const QString& path)
{
	bool isValid = false;
	bool isFile = false;
	bool isAbs = false;

	if (!path.isEmpty())
	{
		QString absPath = Project::GetInstance()->GetAbsoluteFileName(path.trimmed());
		QFileInfo fileInfo(absPath);
	
		if (fileInfo.exists())
		{
			m_Ui.ProjectTree->SetCurrentFile(Project::GetInstance()->GetRelativeFileName(absPath));
			isValid = true;

			isFile = fileInfo.isFile() && m_FilterExtensions.contains(fileInfo.suffix().toLower());
			isAbs = QDir::isAbsolutePath(path);
		}
	}

	m_Ui.OkButton->setEnabled(isFile);

	QPalette palette = m_Ui.FileName->palette();
	QColor textColor = Qt::black;
	if (isValid)
	{
		if (isAbs) textColor = Qt::red;
	}
	else textColor = Qt::darkGray;
	palette.setColor(QPalette::Active, QPalette::Text, textColor);
	m_Ui.FileName->setPalette(palette);	
}

//////////////////////////////////////////////////////////////////////////
QString FilePickerDialog::GetCurrentFile() const
{
	return QDir::fromNativeSeparators(m_Ui.FileName->text().trimmed());
}

//////////////////////////////////////////////////////////////////////////
void FilePickerDialog::SetCurrentFile(const QString& fileName)
{
	m_Ui.FileName->setText(QDir::fromNativeSeparators(fileName));
}

//////////////////////////////////////////////////////////////////////////
void FilePickerDialog::SetFilterTypes(const QString& types)
{
	m_FilterTypes = types.split(";", QString::SkipEmptyParts);
	QtUtil::FileTypeListToExtList(types, m_FilterExtensions);

	QStringList masks;
	qforeach (const QString& ext, m_FilterExtensions)
	{
		masks << "*." + ext;
	}
	m_Ui.ProjectTree->SetFilters(masks);
}


} // namespace Armed