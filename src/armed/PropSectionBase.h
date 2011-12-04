// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef PROPSECTIONBASE_H
#define PROPSECTIONBASE_H

#include <QWidget>


namespace Armed
{
	class PropSectionBase : public QWidget
	{
		Q_OBJECT

	public:
		PropSectionBase(QWidget* parent);
		~PropSectionBase();

		virtual void DisplayData() = 0;

	private:
	
	};
}

#endif // PROPSECTIONBASE_H
