// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedSceneNodeNavigator_H__
#define __ArmedSceneNodeNavigator_H__


#include <QObject>


namespace Armed
{
	class DocScene;
	class SceneNodeSortFilterModel;
	class FilterWidget;

	class SceneNodeNavigator : public QWidget
	{
		Q_OBJECT

	public:
		SceneNodeNavigator(QWidget* parent, DocScene* docScene);
		~SceneNodeNavigator();

		void FitColumns();

	public slots:
		void SelectIndexRange(const QModelIndexList& indexes);

	private slots:
		void ResetSort();
		void ReApplySort();
		void OnDataChanged();
		void OnSelectionChanged();
		void OnFilterChanged(const QString& filterStr);

	private:
		QTreeView* m_SceneTree;
		FilterWidget* m_FilterWidget;
		DocScene* m_DocScene;

		SceneNodeSortFilterModel* m_SortFilterModel;

		bool m_IgnoreSelChanges;
	};
}

#endif // __ArmedSceneNodeNavigator_H__
