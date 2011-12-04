// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedPropertyColorWidget_H__
#define __ArmedPropertyColorWidget_H__


#include "GenericPropertyWidget.h"


using namespace Wme;

namespace Wme
{
	class PropertyValueColor;
}

namespace Armed
{
	class ColorButton;

	class PropertyColorWidget : public GenericPropertyWidget
	{
		Q_OBJECT

	public:
		PropertyColorWidget(QWidget* parent, PropertyValueColor* value);
		~PropertyColorWidget();

	private slots:
		void OnColorChanged(const QColor& color);

	private:
		PropertyValueColor* m_Value;
		ColorButton* m_ColorButton;
	
	};
}

#endif // __ArmedPropertyColorWidget_H__
