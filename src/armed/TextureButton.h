// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedTextureButton_H__
#define __ArmedTextureButton_H__


#include <QPushButton>


namespace Armed
{
	class TextureButton : public QPushButton
	{
		Q_OBJECT

	public:
		TextureButton(QWidget* parent = NULL);
		~TextureButton();

		void SetImage(const QString& fileName);

	signals:
		void TextureChanged(const QString& fileName);

	protected:
		void paintEvent(QPaintEvent* event);
		void dragEnterEvent(QDragEnterEvent* event);
		void dropEvent(QDropEvent* event);

		QSize sizeHint() const;

	private slots:
		void OnClicked();

	private:
		QImage m_Thumbnail;
		QString m_FileName;
		
		static const int arrowSize = 8;
		static const int border = 4;
		static const int imageSize = 60;

		static QString allowedTypes;
	};
}

#endif // __ArmedTextureButton_H__
