// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedFilePickerDialog_H__
#define __ArmedFilePickerDialog_H__


#include <QDialog>
#include "ui_FilePickerDialog.h"


namespace Armed
{
	class FilePickerDialog : public QDialog
	{
		Q_OBJECT

	public:
		FilePickerDialog(QWidget* parent = 0);
		~FilePickerDialog();

		QString GetCurrentFile() const;
		void SetCurrentFile(const QString& fileName);

		void SetFilterTypes(const QString& types);

	private slots:
		void OnFinished();
		void OnPathSelected(const QString& path);
		void OnPathEdited(const QString& path);

	private:
		Ui::FilePickerDialog m_Ui;
		QStringList m_FilterTypes;
		QSet<QString> m_FilterExtensions;
	};
}

#endif // __ArmedFilePickerDialog_H__
