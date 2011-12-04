// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedDocumentContainer_H__
#define __ArmedDocumentContainer_H__


#include <QObject>


namespace Armed
{
	class MainWindow;
	class DocumentView;

	class DocumentContainer : public QWidget
	{
		Q_OBJECT

	public:
		DocumentContainer(MainWindow* parent);
		~DocumentContainer();

		void AddDocumentView(DocumentView* doc, bool activate = true);
		bool CanClose(DocumentView* singleDoc = NULL);

		void TestData();

	private slots:
		void OnDocumentChanged(int index);
		void OnTitleChanged(const QString& title, const QString& desc);
		void OnCloseTab();
		void OnCloseTab(int index);
		void OnTabMenu(QPoint point);

	private:
		QTabWidget* m_TabWidget;
		QToolButton* m_TabCloseButton;
	};
}

#endif // __ArmedDocumentContainer_H__
