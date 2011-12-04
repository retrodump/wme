// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __ArmedPropertyTextureWidget_H__
#define __ArmedPropertyTextureWidget_H__


#include "GenericPropertyWidget.h"


using namespace Wme;

namespace Wme
{
	class PropertyValueTexture;
}

namespace Armed
{
	class TextureButton;

	class PropertyTextureWidget : public GenericPropertyWidget
	{
		Q_OBJECT

	public:
		PropertyTextureWidget(QWidget* parent, PropertyValueTexture* value);
		~PropertyTextureWidget();

		private slots:
			void OnTextureChanged(const QString& fileName);

	private:
		PropertyValueTexture* m_Value;
		TextureButton* m_TextureButton;

	};
}

#endif // __ArmedPropertyTextureWidget_H__
