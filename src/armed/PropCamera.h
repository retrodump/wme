// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef PROPCAMERA_H
#define PROPCAMERA_H

#include "PropSectionBase.h"
#include "ui_PropCamera.h"

using namespace Wme;

namespace Wme
{
	class CameraEntity;
}

namespace Armed
{
	class PropCamera : public PropSectionBase
	{
		Q_OBJECT

	public:
		PropCamera(QWidget *parent = 0);
		~PropCamera();

		void DisplayData();
		void SetCamera(CameraEntity* camera);

	private slots:
		void OnProjectionTypeChanged();
		void OnOrthoHeightChanged();
		void OnFovChanged();
		void OnFovSliderChanged();
		void OnClipNearChanged();
		void OnClipFarChanged();
		
	private:
		void SetState();

		Ui::PropCamera ui;
		bool m_IsDisplayingData;
		CameraEntity* m_Camera;
	};
}

#endif // PROPCAMERA_H
