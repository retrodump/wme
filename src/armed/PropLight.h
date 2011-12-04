// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef PROPLIGHT_H
#define PROPLIGHT_H

#include "PropSectionBase.h"
#include "ui_PropLight.h"


using namespace Wme;

namespace Wme
{
	class LightEntity;
}


namespace Armed
{
	class PropLight : public PropSectionBase
	{
		Q_OBJECT

	public:
		PropLight(QWidget* parent = 0);
		~PropLight();

		void DisplayData();
		void SetLight(LightEntity* light);

	private slots:
		void OnLightTypeChanged();
		void OnDiffuseColorChanged();
		void OnSpecularColorChanged();
		void OnAttenuationChanged();
		void OnSpotlightChanged();
		void OnCastShadowsChanged();

	private:
		Ui::PropLight ui;
		bool m_IsDisplayingData;
		LightEntity* m_Light;

		void SetState();
	};
}

#endif // PROPLIGHT_H
