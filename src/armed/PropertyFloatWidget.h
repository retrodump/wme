// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedPropertyFloatWidget_H__
#define __ArmedPropertyFloatWidget_H__


#include "GenericPropertyWidget.h"


using namespace Wme;

namespace Wme
{
	class PropertyValueFloat;
}

namespace Armed
{
	class PropertyFloatWidget : public GenericPropertyWidget
	{
		Q_OBJECT

	public:
		PropertyFloatWidget(QWidget* parent, PropertyValueFloat* value);
		~PropertyFloatWidget();

	private slots:
		void OnValueChanged(double val);

	private:
		PropertyValueFloat* m_Value;
		QDoubleSpinBox* m_SpinBox;
	
	};
}

#endif // __ArmedPropertyFloatWidget_H__
