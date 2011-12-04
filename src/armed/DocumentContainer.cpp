// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "StdAfx.h"
#include "DocumentContainer.h"
#include "MainWindow.h"
#include "Navigator.h"
#include "PropWindow.h"
#include "WmeWidget.h"
#include "DocumentView.h"
#include "ActionManager.h"
#include "DocumentView.h"
#include "CanCloseDialog.h"

// temp
#include "DocScript.h"
#include "DocScene.h"


namespace Armed
{


//////////////////////////////////////////////////////////////////////////
DocumentContainer::DocumentContainer(MainWindow* parent) : QWidget(parent)
{
	QVBoxLayout* vboxLayout = new QVBoxLayout(this);
	vboxLayout->setMargin(0);

	m_TabWidget = new QTabWidget(this);
#ifdef Q_OS_MAC
	m_TabWidget->setDocumentMode(true);
#endif
	m_TabWidget->setMovable(true);
	
	vboxLayout->addWidget(m_TabWidget);
	connect(m_TabWidget, SIGNAL(currentChanged(int)), this, SLOT(OnDocumentChanged(int)));

	// close button
	QToolButton* m_TabCloseButton = new QToolButton(this);
	m_TabCloseButton->setEnabled(true);
	m_TabCloseButton->setAutoRaise(true);
	m_TabCloseButton->setToolTip(tr("Close"));
	m_TabCloseButton->setIcon(QIcon(":/icons/close.png"));

	m_TabWidget->setCornerWidget(m_TabCloseButton, Qt::TopRightCorner);
	connect(m_TabCloseButton, SIGNAL(clicked()), this, SLOT(OnCloseTab()));

	connect(m_TabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(OnCloseTab(int)));
	new QShortcut(QKeySequence::Close, this, SLOT(OnCloseTab()));


	// context menu
	QTabBar* tabBar = m_TabWidget->findChild<QTabBar*>();
	if (tabBar)
	{
		tabBar->setContextMenuPolicy(Qt::CustomContextMenu);
		connect(tabBar, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(OnTabMenu(QPoint)));
	}
}

//////////////////////////////////////////////////////////////////////////
DocumentContainer::~DocumentContainer()
{

}

//////////////////////////////////////////////////////////////////////////
void DocumentContainer::AddDocumentView(DocumentView* doc, bool activate)
{
	int index = m_TabWidget->addTab(doc, doc->GetTitle());
	m_TabWidget->setCurrentIndex(index);

	connect(doc, SIGNAL(TitleChanged(QString, QString)), this, SLOT(OnTitleChanged(QString, QString)));
	doc->OnDocumentAdded();
}

//////////////////////////////////////////////////////////////////////////
void DocumentContainer::OnDocumentChanged(int index)
{
	MainWindow::GetInstance()->GetNavigator()->SetNavigator(NULL);
	MainWindow::GetInstance()->GetPropWindow()->ClearProperties();

	DocumentView* doc = qobject_cast<DocumentView*>(m_TabWidget->widget(index));
	ActionManager::GetInstance()->SetContextObject(ActionContext::CONTEXT_DOC, doc);

	if (doc) doc->OnActivate();
}

//////////////////////////////////////////////////////////////////////////
void DocumentContainer::OnTitleChanged(const QString& title, const QString& desc)
{
	int index = m_TabWidget->indexOf(qobject_cast<QWidget*>(sender()));
	if (index >= 0)
	{
		m_TabWidget->setTabText(index, title);
		m_TabWidget->setTabToolTip(index, desc);
	}
}

//////////////////////////////////////////////////////////////////////////
void DocumentContainer::OnCloseTab()
{
	OnCloseTab(m_TabWidget->currentIndex());
}

//////////////////////////////////////////////////////////////////////////
void DocumentContainer::OnCloseTab(int index)
{
	DocumentView* doc = qobject_cast<DocumentView*>(m_TabWidget->widget(index));
	if (!doc) return;

	if (CanClose(doc))
	{
		m_TabWidget->removeTab(m_TabWidget->indexOf(doc));
		QTimer::singleShot(0, doc, SLOT(deleteLater()));
	}
}

//////////////////////////////////////////////////////////////////////////
bool DocumentContainer::CanClose(DocumentView* singleDoc)
{
	QList<DocumentView*> dirtyDocs;
	if (singleDoc)
	{
		if (singleDoc->IsDirty()) dirtyDocs.append(singleDoc);
	}
	else
	{
		for (int i = 0; i < m_TabWidget->count(); i++)
		{
			DocumentView* doc = qobject_cast<DocumentView*>(m_TabWidget->widget(i));
			if (doc && doc->IsDirty()) dirtyDocs.append(doc);
		}
	}

	if (dirtyDocs.empty()) return true;

	CanCloseDialog dlg(this);
	qforeach (DocumentView* doc, dirtyDocs)
	{
		dlg.AddFile(doc->GetTitle(false));
	}

	if (!dlg.exec()) return false;

	if (dlg.GetSave())
	{
		qforeach (DocumentView* doc, dirtyDocs)
		{
			if (!doc->Save()) return false;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
void DocumentContainer::OnTabMenu(QPoint point)
{
	QTabBar* tabBar = m_TabWidget->findChild<QTabBar*>();
	if (!tabBar) return;

	// find the tab / document we clicked
	DocumentView* doc = NULL;
	for (int i = 0; i < tabBar->count(); ++i)
	{
		if (tabBar->tabRect(i).contains(point))
		{
			doc = qobject_cast<DocumentView*>(m_TabWidget->widget(i));
			break;
		}
	}
	if (!doc) return;


	QMenu menu(QLatin1String(""), tabBar);
	QAction* closeTab = menu.addAction(tr("Close"));

	QAction* pickedAction = menu.exec(tabBar->mapToGlobal(point));
	if (pickedAction == closeTab)
	{
		OnCloseTab(m_TabWidget->indexOf(doc));
	}
}

//////////////////////////////////////////////////////////////////////////
void DocumentContainer::TestData()
{
	// temp

	DocScene* scene = new DocScene(this);
	scene->BuildActions();
	//scene->Load("F:\\aaa.scene");
	AddDocumentView(scene);

	DocScript* doc = new DocScript(this);
	doc->BuildActions();
	doc->Load("../data/test.script");
	AddDocumentView(doc);

	m_TabWidget->setCurrentIndex(0);
}


} // namespace Armed
