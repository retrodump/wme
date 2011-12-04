// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedPropMaterial_H__
#define __ArmedPropMaterial_H__


#include "PropSectionBase.h"


using namespace Wme;

namespace Wme
{
	class MeshEntity;
}

namespace Armed
{
	class MaterialWidget;

	class PropMaterial : public PropSectionBase
	{
		Q_OBJECT

	public:
		PropMaterial(QWidget* parent = 0);
		~PropMaterial();

		void SetEntity(MeshEntity* ent);

	public slots:
		void DisplayData();

	private slots:
		void OnRebuildNeeded();

	private:
		MeshEntity* m_Entity;

		QVBoxLayout* m_Layout;

		void Clear();
		QVector<MaterialWidget*> m_Items;
	};
}

#endif // __ArmedPropMaterial_H__
