// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "DocumentView.h"
#include "MainWindow.h"
#include "ActionManager.h"


namespace Armed
{

	
//////////////////////////////////////////////////////////////////////////
DocumentView::DocumentView(QWidget* parent) : QWidget(parent)
{
	m_IsDirty = false;
	m_SaveAction = NULL;
	m_SaveAsAction = NULL;
}

//////////////////////////////////////////////////////////////////////////
DocumentView::~DocumentView()
{

}

//////////////////////////////////////////////////////////////////////////
void DocumentView::OnDocumentAdded()
{
	emit TitleChanged(GetTitle(), GetDescription());
}

//////////////////////////////////////////////////////////////////////////
void DocumentView::SetFileName(const QString& fileName)
{
	if (m_FileName != fileName)
	{
		m_FileName = fileName;
		emit TitleChanged(GetTitle(), GetDescription());
	}
}

//////////////////////////////////////////////////////////////////////////
void DocumentView::SetDirty(bool value)
{
	if (m_IsDirty != value)
	{
		m_IsDirty = value;
		emit TitleChanged(GetTitle(), GetDescription());
	}
}

//////////////////////////////////////////////////////////////////////////
bool DocumentView::Load(const QString& fileName)
{
	if (LoadData(fileName))
	{
		SetFileName(fileName);
		SetDirty(false);
		return true;
	}
	else return false;
}

//////////////////////////////////////////////////////////////////////////
bool DocumentView::Save()
{
	if (m_FileName.isEmpty()) return SaveAs();
	else
	{
		bool ret = SaveData(m_FileName);
		if (ret) SetDirty(false);

		return ret;
	}
}

//////////////////////////////////////////////////////////////////////////
bool DocumentView::SaveAs()
{
	QString filters;
	filters += GetDocDescription() + " (*." + GetDocExtension() + ");;";
	filters += tr("All files") + " (*)";

	QString newFileName = QFileDialog::getSaveFileName(this, tr("Save file as"), m_FileName, filters);
	if (newFileName.isEmpty()) return false;

	if (SaveData(newFileName))
	{
		SetFileName(newFileName);
		SetDirty(false);
		return true;
	}
	else return false;
}

//////////////////////////////////////////////////////////////////////////
QString DocumentView::GetTitle(bool includeModifyMark) const
{
	QString title;

	if (m_FileName.isEmpty()) title = GetDefaultName();
	else title = QFileInfo(GetFileName()).fileName();

	if (includeModifyMark)
		title += (IsDirty() ? "*" : "");

	return title;
}

//////////////////////////////////////////////////////////////////////////
QString DocumentView::GetDescription() const
{
	QString desc;

	if (m_FileName.isEmpty()) desc = GetDefaultName();
	else desc = GetFileName();

	desc += (IsDirty() ? "*" : "");

	return desc;
}

//////////////////////////////////////////////////////////////////////////
void DocumentView::BuildActions()
{
	ActionManager* am = ActionManager::GetInstance();

	m_SaveAction = new QAction(tr("Save"), this);
	m_SaveAction->setStatusTip(tr("Save document"));
	m_SaveAction->setIcon(QIcon(":/icons/disk.png"));
	m_SaveAction->setShortcut(QKeySequence::Save);
	connect(m_SaveAction, SIGNAL(triggered()), this, SLOT(Save()));
	am->AddAction("File.Save", m_SaveAction, ActionContext::CONTEXT_DOC);

	m_SaveAsAction = new QAction(tr("Save As..."), this);
	m_SaveAsAction->setStatusTip(tr("Save document as"));
	m_SaveAsAction->setShortcut(QKeySequence::SaveAs);
	connect(m_SaveAsAction, SIGNAL(triggered()), this, SLOT(SaveAs()));
	am->AddAction("File.SaveAs", m_SaveAsAction, ActionContext::CONTEXT_DOC);
}

//////////////////////////////////////////////////////////////////////////
void DocumentView::ReadError(const QString& fileName)
{
	QMessageBox::critical(this, MainWindow::GetAppTitle(), tr("Error reading from file '%1'.").arg(fileName));
}

//////////////////////////////////////////////////////////////////////////
void DocumentView::WriteError(const QString& fileName)
{
	QMessageBox::critical(this, MainWindow::GetAppTitle(), tr("Error writing to file '%1'.").arg(fileName));
}


} // namespace Armed
