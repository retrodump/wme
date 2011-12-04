// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedProjectTreeView_H__
#define __ArmedProjectTreeView_H__


#include <QTreeView>


namespace Armed
{
	class ProjectTreeModel;
	class ProjectTreeFilter;

	class ProjectTreeView : public QTreeView
	{
		Q_OBJECT

	public:
		ProjectTreeView(QWidget* parent);
		~ProjectTreeView();

		QString GetCurrentFile() const;
		void SetCurrentFile(const QString& fileName);

		void SetProjectRoot();

		void SetFilters(const QStringList& filters);
		void SetUserFilter(const QString& filter);

	signals:
		void PathSelected(const QString& path);

	private slots:
		void OnSelectionChanged();
		void ScrollToCurrent();

	private:
		ProjectTreeFilter* m_FilterModel;
		ProjectTreeModel* m_Model;
		
		void PopulatePath(const QString& path);
	};
}

#endif // __ArmedProjectTreeView_H__
