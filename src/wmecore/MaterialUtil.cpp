// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "MaterialUtil.h"
#include "StringUtil.h"


namespace Wme
{


const Utf8String MaterialUtil::HIGHLIGHT_MAT_PREFIX = "sys/highlight/";


//////////////////////////////////////////////////////////////////////////
bool MaterialUtil::IsHighlightedName(const Utf8String& matName)
{
	return Ogre::StringUtil::startsWith(matName, HIGHLIGHT_MAT_PREFIX);
}

//////////////////////////////////////////////////////////////////////////
Utf8String MaterialUtil::GetHighlightedName(const Utf8String& origName)
{
	return HIGHLIGHT_MAT_PREFIX + origName;
}

//////////////////////////////////////////////////////////////////////////
Utf8String MaterialUtil::GetUnhighlightedName(const Utf8String& origName)
{
	return origName.substr(HIGHLIGHT_MAT_PREFIX.length());
}

//////////////////////////////////////////////////////////////////////////
Ogre::MaterialPtr MaterialUtil::GetHighlightMat(const Ogre::MaterialPtr origMat, bool forceRebuild)
{
	Utf8String hlName = GetHighlightedName(origMat->getName());

	Ogre::MaterialPtr hlMat = Ogre::MaterialManager::getSingleton().getByName(hlName);
	if (hlMat.isNull())
	{
		hlMat = origMat->clone(hlName);
		forceRebuild = true;
	}
	else
	{
		if (forceRebuild) origMat->copyDetailsTo(hlMat);
	}

	if (forceRebuild)
	{
		Ogre::Pass* pass = hlMat->getTechnique(0)->createPass();
		
		pass->setPolygonMode(Ogre::PM_WIREFRAME);
		
		//pass->setDepthBias(0.1f, 0.01f);
		pass->setDepthBias(0.2f, 0.2f);
		
		pass->setLightingEnabled(true);
		pass->setAmbient(Ogre::ColourValue::Black);
		pass->setDiffuse(Ogre::ColourValue::Black);
		pass->setSpecular(Ogre::ColourValue::Black);
		pass->setSelfIllumination(0.8f, 0.8f, 0.8f); // TODO configurable color

		pass->setFog(true, Ogre::FOG_NONE);
	}

	return hlMat;
}

//////////////////////////////////////////////////////////////////////////
Ogre::MaterialPtr MaterialUtil::GetEmptyMat()
{
	return Ogre::MaterialManager::getSingleton().getByName("BaseWhite");
}


} // namespace Wme