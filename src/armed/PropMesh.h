// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef PROPMESH_H
#define PROPMESH_H

#include "PropSectionBase.h"
#include "ui_PropMesh.h"

using namespace Wme;

namespace Wme
{
	class MeshEntity;
}

namespace Armed
{
	class PropMesh : public PropSectionBase
	{
		Q_OBJECT

	public:
		PropMesh(QWidget* parent = 0);
		~PropMesh();

		void DisplayData();
		void SetEntity(MeshEntity* entity);

	signals:
		void MeshChanged();
		void SkeletonChanged();

	private slots:
		void OnMeshChanged();
		void OnSkeletonChanged();
		void OnRemoveSkeleton();

	private:
		void SetState();

		Ui::PropMesh ui;
		bool m_IsDisplayingData;
		MeshEntity* m_Entity;
	};
}

#endif // PROPMESH_H
