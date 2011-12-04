// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedProjectBrowser_H__
#define __ArmedProjectBrowser_H__


#include <QWidget>


namespace Armed
{
	class ProjectTreeView;
	class FilterWidget;

	class ProjectBrowser : public QWidget
	{
		Q_OBJECT

	public:
		ProjectBrowser(QWidget* parent);
		~ProjectBrowser();

	private slots:
		void OnProjectChanged();
		void OnFilterChanged(const QString& filterStr);

	private:
		ProjectTreeView* m_ProjectTree;
		FilterWidget* m_FilterWidget;
	};
}

#endif // __ArmedProjectBrowser_H__
