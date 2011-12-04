// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedCanCloseDialog_H__
#define __ArmedCanCloseDialog_H__


#include <QWidget>
#include "ui_CanCloseDialog.h"


namespace Armed
{
	class CanCloseDialog : public QDialog
	{
		Q_OBJECT

	public:
		CanCloseDialog(QWidget* parent = 0);
		~CanCloseDialog();

		void AddFile(const QString& fileName);
		bool GetSave() const { return m_Save; }

	private slots:
		void OnYes();
		void OnNo();

	private:
		Ui::CanCloseDialogClass m_Ui;
		bool m_Save;
	};
}

#endif // __ArmedCanCloseDialog_H__
