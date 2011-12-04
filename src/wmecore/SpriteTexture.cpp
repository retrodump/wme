// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "SpriteTexture.h"
#include "StringUtil.h"
#include "OgreBitwise.h"
#include "Game.h"

namespace Wme
{


//////////////////////////////////////////////////////////////////////////
SpriteTexture::SpriteTexture()
{
	m_Width = m_Height = 0;
	m_RealWidth = m_RealHeight = 0;
	m_ColorKey = Ogre::ColourValue::ZERO;
}


//////////////////////////////////////////////////////////////////////////
SpriteTexture::~SpriteTexture()
{
}

//////////////////////////////////////////////////////////////////////////
Ogre::Real SpriteTexture::GetTexCoordU() const
{
	return (Ogre::Real)m_Width / (Ogre::Real)m_RealWidth;
}

//////////////////////////////////////////////////////////////////////////
Ogre::Real SpriteTexture::GetTexCoordV() const
{
	return (Ogre::Real)m_Height / (Ogre::Real)m_RealHeight;
}

//////////////////////////////////////////////////////////////////////////
void SpriteTexture::DestroyMaterial()
{
	if (!m_Material.isNull())
	{
		Ogre::MaterialManager::getSingleton().remove(m_Material->getHandle());
		m_Material.setNull();
	}

	if (!m_Texture.isNull())
	{
		Ogre::TextureManager::getSingleton().remove(m_Texture->getHandle());
		m_Texture.setNull();
	}

	m_FileName.clear();
	m_Width = m_Height = 0;
	m_RealWidth = m_RealHeight;
}

//////////////////////////////////////////////////////////////////////////
bool SpriteTexture::SetTexture(const WideString& fileName, Ogre::ColourValue colorKey)
{
	DestroyMaterial();

	try
	{
		Utf8String fileNameUtf8 = StringUtil::WideToUtf8(fileName);

		const Ogre::RenderSystemCapabilities* caps = Ogre::Root::getSingleton().getRenderSystem()->getCapabilities();
		/*
		// no chroma key and hw supports non-pow2 textures - simply load		
		if (caps->hasCapability(Ogre::RSC_NON_POWER_OF_2_TEXTURES) && colorKey == Ogre::ColourValue::ZERO)
		{
			m_Texture = Ogre::TextureManager::getSingleton().load(fileNameUtf8, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D, 0);

			m_Width = m_RealWidth = m_Texture->getWidth();
			m_Height = m_RealHeight = m_Texture->getHeight();
		}
		// otherwise create pow-2 texture and remember original size
		// and apply chroma-keying if requested
		else
		*/
		{
			Ogre::Image img;

			// load image
			if (colorKey != Ogre::ColourValue::ZERO)
			{
				Ogre::Image tempImg;
				tempImg.load(fileNameUtf8, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

				size_t width = tempImg.getWidth();
				size_t height = tempImg.getHeight();

				byte* pixelData = OGRE_ALLOC_T(byte, Ogre::PixelUtil::getMemorySize(width, height, 1, Ogre::PF_A8R8G8B8), Ogre::MEMCATEGORY_GENERAL);
				size_t pxDataIndex = 0;
				size_t pxDataIndexStep = Ogre::PixelUtil::getNumElemBytes(Ogre::PF_A8R8G8B8); 

				for (size_t y = 0; y < height; y++)
				{ 
					for (size_t x = 0; x < width; x++) 
					{ 
						Ogre::ColourValue pixCol = tempImg.getColourAt(x, y, 0); 
						if (pixCol == colorKey)
						{
							//pixCol = Ogre::ColourValue::ZERO;
							pixCol = Ogre::ColourValue(0.1f, 0.1f, 0.1f, 0.0f);
						}
						else
						{
							pixCol.a = 1.0f;
						}
						Ogre::PixelUtil::packColour(pixCol, Ogre::PF_A8R8G8B8, static_cast<void*>(pixelData + pxDataIndex)); 
						pxDataIndex += pxDataIndexStep; 
					} 
				} 
				img.loadDynamicImage(pixelData, width, height, 1, Ogre::PF_A8R8G8B8, true);
			}
			else
			{
				img.load(fileNameUtf8, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
			}
			

			// create (resized) texture
			size_t srcWidth = img.getWidth();
			size_t srcHeight = img.getHeight();

			size_t newWidth;
			size_t newHeight;
			if (!caps->hasCapability(Ogre::RSC_NON_POWER_OF_2_TEXTURES))
			{
				newWidth = Ogre::Bitwise::firstPO2From((Ogre::uint32)srcWidth);
				newHeight = Ogre::Bitwise::firstPO2From((Ogre::uint32)srcHeight);
			}
			else
			{
				newWidth = srcWidth;
				newHeight = srcHeight;
			}

			m_Texture = Ogre::TextureManager::getSingleton().createManual(
				fileNameUtf8,
				Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
				Ogre::TEX_TYPE_2D,
				newWidth,
				newHeight,
				0,
				Ogre::PF_A8R8G8B8,
				Ogre::TU_AUTOMIPMAP | Ogre::TU_STATIC); // must NOT be write-only

			Ogre::HardwarePixelBufferSharedPtr buf = m_Texture->getBuffer();
			buf->blitFromMemory(img.getPixelBox(), Ogre::Box(0,0, srcWidth, srcHeight));

			m_Width = srcWidth;
			m_Height = srcHeight;

			m_RealWidth = newWidth;
			m_RealHeight = newHeight;
		}


		// create material
		m_Material = Ogre::MaterialManager::getSingleton().create(fileNameUtf8, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

		Ogre::Pass* pass = m_Material->getTechnique(0)->getPass(0);
		Ogre::TextureUnitState* texLayer = pass->createTextureUnitState(fileNameUtf8);
		
		// clamp to avoid fuzzy edges
		texLayer->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
		
		// allow min/mag filter, but no mip
		texLayer->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_NONE);

		// set up blending
		if (m_Texture->hasAlpha())
		{
			m_Material->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
		}
		else
		{
			// use add if no alpha (assume black background)
			m_Material->setSceneBlending(Ogre::SBT_ADD);
		}


		m_Material->setCullingMode(Ogre::CULL_NONE);
		m_Material->setFog(Ogre::FOG_NONE);
		m_Material->setLightingEnabled(false);
		m_Material->setDepthCheckEnabled(false);
		m_Material->setDepthWriteEnabled(false);

		texLayer->setColourOperationEx(Ogre::LBX_MODULATE, Ogre::LBS_TEXTURE, Ogre::LBS_DIFFUSE);
		texLayer->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_TEXTURE, Ogre::LBS_DIFFUSE);
		//pass->setAlphaRejectSettings(Ogre::CMPF_GREATER_EQUAL, 0, false);


		// remember parameters for serialization purposes
		m_FileName = fileName;
		m_ColorKey = colorKey;

		return true;
	}
	catch (Ogre::Exception& e)
	{
		Game::GetInstance()->Log(L"Error loading texture '" + fileName + L"'.");
		Game::GetInstance()->Log(e.getDescription());
		return false;
	}	
}

//////////////////////////////////////////////////////////////////////////
bool SpriteTexture::IsTransparentAt(int posX, int posY)
{
	if (posX < 0 || posX >= (int)m_Width || posY < 0 || posY >= (int)m_Height) return true;

	Ogre::HardwarePixelBufferSharedPtr pixelBuffer = m_Texture->getBuffer();
	pixelBuffer->lock(0, pixelBuffer->getSizeInBytes(), Ogre::HardwareBuffer::HBL_READ_ONLY);
	const Ogre::PixelBox& pixelBox = pixelBuffer->getCurrentLock();	
	byte* src = static_cast<byte*>(pixelBox.data) + ((posY * pixelBox.rowPitch + posX) * Ogre::PixelUtil::getNumElemBytes(pixelBox.format));

	Ogre::ColourValue pixel;
	Ogre::PixelUtil::unpackColour(&pixel, pixelBox.format, static_cast<void*>(src));

	pixelBuffer->unlock();

	return pixel.a < 0.5;
}


} // namespace Wme
