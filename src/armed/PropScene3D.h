// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef PROPSCENE3D_H
#define PROPSCENE3D_H

#include "PropSectionBase.h"
#include "ui_PropScene3D.h"


using namespace Wme;

namespace Wme
{
	class Scene3DBase;
}

namespace Armed
{
	class PropScene3D : public PropSectionBase
	{
		Q_OBJECT

	public:
		PropScene3D(QWidget* parent = 0);
		~PropScene3D();

		void SetScene(Scene3DBase* scene);
		void DisplayData();

	private slots:
		void OnAmbientColorChanged();
		void OnFogTypeChanged();
		void OnFogColorChanged();
		void OnFogDensityChanged();
		void OnFogStartChanged();
		void OnFogEndChanged();

	private:
		Ui::PropScene3D ui;
		void SetState();

		bool m_IsDisplayingData;
		Scene3DBase* m_Scene;
	};
}

#endif // PROPSCENE3D_H
