// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef ENTITYMATERIALWIDGET_H
#define ENTITYMATERIALWIDGET_H

#include "MaterialWidget.h"


using namespace Wme;

namespace Wme
{
	class MeshEntity;
}


namespace Armed
{
	class EntityMaterialWidget : public MaterialWidget
	{
		Q_OBJECT

	public:
		EntityMaterialWidget(QWidget* parent);
		~EntityMaterialWidget();

		void SetSubEntity(MeshEntity* ent, int subEnt, bool isLast);

	protected:
		virtual void ResetMaterial(const QString& fileName);
		virtual void ReApplyMaterial();

	private:
		MaterialInstance* GetCurrentMaterial();
		MeshEntity* m_Entity;
		int m_SubEnt;			
	};
}

#endif // ENTITYMATERIALWIDGET_H
