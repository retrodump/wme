// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef PROPSKYBOX_H
#define PROPSKYBOX_H

#include "PropSectionBase.h"


using namespace Wme;

namespace Wme
{
	class Scene3DBase;
}

namespace Armed
{
	class SkyboxMaterialWidget;

	class PropSkybox : public PropSectionBase
	{
		Q_OBJECT

	public:
		PropSkybox(QWidget* parent = 0);
		~PropSkybox();

		void SetScene(Scene3DBase* scene);

	public slots:
		void DisplayData();

	private slots:
		void OnRebuildNeeded();


	private:
		Scene3DBase* m_Scene;

		QVBoxLayout* m_Layout;
		SkyboxMaterialWidget* m_MaterialWidget;
	};
}

#endif // PROPSKYBOX_H
