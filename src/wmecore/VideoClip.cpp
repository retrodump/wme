// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "VideoClip.h"
#include "VideoManager.h"
#include "TheoraVideoManager.h"
#include "TheoraVideoClip.h"
#include "TheoraVideoFrame.h"
#include "TheoraFileSystemBridge.h"
#include "Game.h"
#include "OgreBitwise.h"
#include "StringUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
VideoClip::VideoClip()
{
	m_TheoraClip = NULL;
}

//////////////////////////////////////////////////////////////////////////
VideoClip::~VideoClip()
{
	Cleanup();
}

//////////////////////////////////////////////////////////////////////////
void VideoClip::Cleanup()
{
	if (m_TheoraClip)
	{
		TheoraVideoManager* mgr = Game::GetInstance()->GetVideoMgr()->GetTheoraMgr();
		mgr->destroyVideoClip(m_TheoraClip);
		m_TheoraClip = NULL;
	}
	m_FileName.clear();
	
	m_Material.setNull();
	m_Texture.setNull();
}

//////////////////////////////////////////////////////////////////////////
bool VideoClip::Initialize(const WideString& fileName, bool looping)
{
	Cleanup();

	TheoraVideoManager* mgr = Game::GetInstance()->GetVideoMgr()->GetTheoraMgr();
	const Ogre::RenderSystemCapabilities* caps = Ogre::Root::getSingleton().getRenderSystem()->getCapabilities();

	bool pow2tex = !caps->hasCapability(Ogre::RSC_NON_POWER_OF_2_TEXTURES);


	try
	{
		Utf8String fileNameUtf8 = StringUtil::WideToUtf8(fileName);

		m_TheoraClip = mgr->createVideoClip(new TheoraFileSystemBridge(fileName), TH_BGRA, 0, pow2tex);

		int width = m_TheoraClip->getWidth();
		int height = m_TheoraClip->getHeight();

		if (pow2tex)
		{
			width = Ogre::Bitwise::firstPO2From((Ogre::uint32)width);
			height = Ogre::Bitwise::firstPO2From((Ogre::uint32)height);
		}

		// create texture
		m_Texture = Ogre::TextureManager::getSingleton().createManual(
			fileNameUtf8,
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			Ogre::TEX_TYPE_2D,
			width,
			height,
			1,
			0,
			Ogre::PF_X8R8G8B8,
			Ogre::TU_DYNAMIC_WRITE_ONLY);

		// clear texture
		byte* texData = (byte*)m_Texture->getBuffer()->lock(Ogre::HardwareBuffer::HBL_DISCARD);
		memset(texData, 0, width * height * 4);
		m_Texture->getBuffer()->unlock();

		m_FileName = fileName;
		SetLooping(looping);
	}
	catch (...)
	{
		return false;
	}


	return true;
}

//////////////////////////////////////////////////////////////////////////
void VideoClip::Update()
{
	if (m_Texture.isNull()) return;


	TheoraVideoFrame* frame = m_TheoraClip->getNextFrame();
	if (frame)
	{
		int width = frame->getStride();
		int height = frame->getHeight();

		byte* texData = (byte*)m_Texture->getBuffer()->lock(Ogre::HardwareBuffer::HBL_DISCARD);
		byte* videoData = frame->getBuffer();

		memcpy(texData, videoData, width * height * 4);

		m_Texture->getBuffer()->unlock();
		m_TheoraClip->popFrame();
	}
}

//////////////////////////////////////////////////////////////////////////
void VideoClip::AttachToMaterial(const WideString& matName, word techniqueLevel, word passLevel, word stateLevel)
{
	Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName(StringUtil::WideToUtf8(matName));
	if (material.isNull()) return;

	AttachToMaterial(material, techniqueLevel, passLevel, stateLevel);
}

//////////////////////////////////////////////////////////////////////////
void VideoClip::AttachToMaterial(Ogre::MaterialPtr& material, word techniqueLevel, word passLevel, word stateLevel)
{
	if (material.isNull()) return;

	Ogre::TextureUnitState* ts = material->getTechnique(techniqueLevel)->getPass(passLevel)->getTextureUnitState(stateLevel);

	ts->setTextureName(StringUtil::WideToUtf8(m_FileName), Ogre::TEX_TYPE_2D);
	ts->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_NONE);
	ts->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);

	// scale tex coords to fit the 0-1 uv range
	Ogre::Matrix4 scaleMat = Ogre::Matrix4::IDENTITY;
	scaleMat.setScale(Ogre::Vector3((float)m_TheoraClip->getWidth() / m_Texture->getWidth(), (float)m_TheoraClip->getHeight() / m_Texture->getHeight(), 1.0f));
	ts->setTextureTransform(scaleMat);
}

//////////////////////////////////////////////////////////////////////////
void VideoClip::SetLooping(bool val)
{
	m_Looping = val;

	if (m_TheoraClip) m_TheoraClip->setAutoRestart(val);
}

} // namespace Wme
