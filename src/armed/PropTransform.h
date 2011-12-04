// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedPropTransform_H__
#define __ArmedPropTransform_H__

#include "PropSectionBase.h"
#include "ui_PropTransform.h"

using namespace Wme;

namespace Wme
{
	class NodeSelection;
}

namespace Armed
{
	class PropTransform : public PropSectionBase
	{
		Q_OBJECT

	public:
		PropTransform(QWidget* parent = 0);
		~PropTransform();

		void SetSelection(NodeSelection* selection);
		void DisplayData();

	private slots:
		void OnPosChanged();
		void OnRotChanged();
		void OnScaleChanged();

	private:
		Ui::PropTransform ui;

		void SetupSpinBox(QDoubleSpinBox* spinBox);		
		bool m_IsDisplayingData;

		Ogre::Real FixAngle(Ogre::Real angle);

		NodeSelection* m_Selection;
	};
}

#endif // __ArmedPropTransform_H__
