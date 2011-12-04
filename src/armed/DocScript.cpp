// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "DocScript.h"
#include "ScriptEditor.h"
#include "SearchBar.h"
#include "ActionManager.h"



namespace Armed
{


//////////////////////////////////////////////////////////////////////////
DocScript::DocScript(QWidget *parent) : DocumentView(parent)
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);

	m_Editor = new ScriptEditor(this);
	connect(m_Editor->document(), SIGNAL(modificationChanged(bool)), this, SLOT(OnModified(bool)));

	// setup search bar
	m_SearchBar = new SearchBar(this);
	m_SearchBar->setVisible(false);
	connect(m_SearchBar, SIGNAL(FindNext()), this, SLOT(FindNext()));
	connect(m_SearchBar, SIGNAL(FindPrev()), this, SLOT(FindPrev()));
	connect(m_SearchBar, SIGNAL(Find(QString, bool)), this, SLOT(Find(QString, bool)));
	connect(m_SearchBar, SIGNAL(EscapePressed()), this, SLOT(FocusEditor()));

	
	layout->addWidget(m_Editor);
	layout->addWidget(m_SearchBar);
}

//////////////////////////////////////////////////////////////////////////
void DocScript::BuildActions()
{
	DocumentView::BuildActions();

	ActionManager* am = ActionManager::GetInstance();

	QAction* findAction = new QAction(tr("Find..."), this);
	findAction->setStatusTip(tr("Find text"));
	findAction->setIcon(QIcon(":/icons/find.png"));
	findAction->setShortcut(QKeySequence::Find);
	connect(findAction, SIGNAL(triggered()), this, SLOT(FindNew()));
	am->AddAction("Edit.Find", findAction, ActionContext::CONTEXT_DOC);

	QAction* findNextAction = new QAction(tr("Find next"), this);
	findNextAction->setStatusTip(tr("Find next occurence of text"));
	findNextAction->setIcon(QIcon(":/icons/find_next.png"));
	findNextAction->setShortcut(QKeySequence::FindNext);
	connect(findNextAction, SIGNAL(triggered()), this, SLOT(FindNext()));
	am->AddAction("Edit.FindNext", findNextAction, ActionContext::CONTEXT_DOC);

	QAction* findPrevAction = new QAction(tr("Find previous"), this);
	findPrevAction->setStatusTip(tr("Find previous occurence of text"));
	findPrevAction->setIcon(QIcon(":/icons/find_prev.png"));
	findPrevAction->setShortcut(QKeySequence::FindPrevious);
	connect(findPrevAction, SIGNAL(triggered()), this, SLOT(FindPrev()));
	am->AddAction("Edit.FindPrev", findPrevAction, ActionContext::CONTEXT_DOC);
}

//////////////////////////////////////////////////////////////////////////
DocScript::~DocScript()
{

}

//////////////////////////////////////////////////////////////////////////
void DocScript::OnModified(bool modified)
{
	SetDirty(modified);
}

//////////////////////////////////////////////////////////////////////////
void DocScript::OnActivate()
{
	m_Editor->setFocus(Qt::OtherFocusReason);
}

//////////////////////////////////////////////////////////////////////////
bool DocScript::LoadData(const QString& fileName)
{
	QFile file(fileName);
	if (!file.open(QFile::ReadOnly))
	{
		ReadError(fileName);
		return false;
	}
	
	QTextStream stream(&file);
	stream.setAutoDetectUnicode(true);
		
	m_Editor->setPlainText(stream.readAll());
	file.close();

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool DocScript::SaveData(const QString& fileName)
{
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		WriteError(fileName);
		return false;
	}

	QTextStream stream(&file);
	stream.setCodec("UTF-8");
	stream.setGenerateByteOrderMark(true);
	stream << m_Editor->toPlainText();

	file.close();

	m_Editor->document()->setModified(false);
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool DocScript::NewData()
{
	return true;
}

//////////////////////////////////////////////////////////////////////////
void DocScript::FocusEditor()
{
	m_Editor->setFocus();
}

//////////////////////////////////////////////////////////////////////////
void DocScript::FindNew()
{
	m_SearchBar->Show();

	QTextCursor cursor = m_Editor->textCursor();
	if (!cursor.hasSelection()) cursor.select(QTextCursor::WordUnderCursor);	
	m_SearchBar->SetText(cursor.selectedText());
}

//////////////////////////////////////////////////////////////////////////
void DocScript::FindNext()
{
	Find(m_SearchBar->GetText(), true, false);
}

//////////////////////////////////////////////////////////////////////////
void DocScript::FindPrev()
{
	Find(m_SearchBar->GetText(), false, false);
}

//////////////////////////////////////////////////////////////////////////
void DocScript::Find(const QString& text, bool forward, bool inPlace)
{
	QTextDocument::FindFlags flags = 0;
	if (!forward) flags |= QTextDocument::FindBackward;
	if (m_SearchBar->GetCaseSensitive()) flags |= QTextDocument::FindCaseSensitively;
	
	QTextCursor startPos = m_Editor->textCursor();
	if (inPlace) startPos.setPosition(std::min(startPos.anchor(), startPos.position()));

	QTextCursor textCursor = m_Editor->document()->find(text, startPos, flags);

	bool found;
	if (!textCursor.isNull())
	{
		found = true;
		m_Editor->setTextCursor(textCursor);
	}
	else
	{
		found = false;
	}

	if (!found && text.isEmpty()) found = true;

	//if (!m_SearchBar->isVisible()) m_SearchBar->Show();
	m_SearchBar->SetColors(found);
}


} // namespace Armed
