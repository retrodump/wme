// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef SKYBOXMATERIALWIDGET_H
#define SKYBOXMATERIALWIDGET_H

#include "MaterialWidget.h"


using namespace Wme;

namespace Wme
{
	class Scene3DBase;
}


namespace Armed
{
	class SkyboxMaterialWidget : public MaterialWidget
	{
		Q_OBJECT

	public:
		SkyboxMaterialWidget(QWidget* parent);
		~SkyboxMaterialWidget();

		void SetScene(Scene3DBase* scene);

	protected:
		virtual void ResetMaterial(const QString& fileName);
		virtual void ReApplyMaterial();

	private:
		MaterialInstance* GetCurrentMaterial();
		Scene3DBase* m_Scene;
	
	};
}

#endif // SKYBOXMATERIALWIDGET_H
