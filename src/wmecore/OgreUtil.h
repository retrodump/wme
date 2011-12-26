// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeOgreUtil_H__
#define __WmeOgreUtil_H__


namespace Wme
{
	class WmeDllExport OgreUtil
	{
	public:
		static void GetMeshInformation(Ogre::Entity* entity, bool getUV, size_t& vertex_count, Ogre::Vector3*& vertices, unsigned short*& sub_indices, Ogre::Vector2*& uv_coords, size_t& index_count, unsigned long*& indices);
		static void GetBoneWorldPosition(Ogre::Entity* entity, Ogre::Bone* bone, Ogre::Vector3& worldPosition, Ogre::Quaternion& worldOrientation);
	};
}

#endif // __WmeOgreUtil_H__
