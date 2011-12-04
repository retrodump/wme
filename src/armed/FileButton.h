// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedFileButton_H__
#define __ArmedFileButton_H__


#include <QPushButton>


namespace Armed
{
	class FileButton : public QPushButton
	{
		Q_OBJECT

	public:
		FileButton(QWidget* parent, const QString& fileName = "");
		~FileButton();

		QString GetFileName() const { return m_FileName; }
		void SetFileName(const QString& fileName);

		QString GetPlaceholderText() const { return m_PlaceholderText; }
		void SetPlaceholderText(const QString& text);

		QString GetAllowedTypes() const { return m_AllowedTypes; }
		void SetAllowedTypes(const QString& allowedTypes) { m_AllowedTypes = allowedTypes; }

	signals:
		void FileChanged(const QString& fileName);

	protected:
		void paintEvent(QPaintEvent* event);

		void dragEnterEvent(QDragEnterEvent* event);
		void dropEvent(QDropEvent* event);
		

		QString m_FileName;
		QString m_AllowedTypes;
		QString m_PlaceholderText;

	private slots:
		void OnClicked();

	private:
	
	};
}

#endif // __ArmedFileButton_H__
