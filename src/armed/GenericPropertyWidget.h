// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedGenericPropertyWidget_H__
#define __ArmedGenericPropertyWidget_H__


#include <QWidget>


namespace Armed
{
	class GenericPropertyWidget : public QWidget
	{
		Q_OBJECT

	public:
		GenericPropertyWidget(QWidget* parent);
		virtual ~GenericPropertyWidget();

	signals:
		void PropertyChanged();

	private:
	
	};
}

#endif // __ArmedGenericPropertyWidget_H__
