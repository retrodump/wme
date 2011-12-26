// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "OgreUtil.h"
#include "OgreTagPoint.h"


namespace Wme
{


// http://www.ogre3d.org/wiki/index.php/Raycasting_to_the_polygon_level
//////////////////////////////////////////////////////////////////////////
void OgreUtil::GetMeshInformation(Ogre::Entity* entity, bool getUV, size_t& vertex_count, Ogre::Vector3*& vertices, unsigned short*& sub_indices, Ogre::Vector2*& uv_coords, size_t& index_count, unsigned long*& indices)
{
	bool added_shared = false;

	size_t current_offset_v = 0;
	size_t next_offset_v = 0;

	size_t current_offset_t = 0;
	size_t next_offset_t = 0;

	size_t index_offset = 0;
	vertex_count = index_count = 0;

	Ogre::Vector3 position = entity->getParentNode()->_getDerivedPosition();
	Ogre::Quaternion orient = entity->getParentNode()->_getDerivedOrientation();
	Ogre::Vector3 scale = entity->getParentNode()->_getDerivedScale();


	Ogre::MeshPtr mesh = entity->getMesh();


	bool useSoftwareBlendingVertices = entity->hasSkeleton();

	if (useSoftwareBlendingVertices)
	{
		entity->_updateAnimation();
	}

	// Calculate how many vertices and indices we're going to need
	for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
	{
		Ogre::SubMesh* submesh = mesh->getSubMesh( i );

		// We only need to add the shared vertices once
		if(submesh->useSharedVertices)
		{
			if( !added_shared )
			{
				vertex_count += mesh->sharedVertexData->vertexCount;
				added_shared = true;
			}
		}
		else
		{
			vertex_count += submesh->vertexData->vertexCount;
		}

		// Add the indices
		index_count += submesh->indexData->indexCount;
	}


	// Allocate space for the vertices and indices
	vertices = new Ogre::Vector3[vertex_count];
	memset(vertices, 0, vertex_count * sizeof(Ogre::Vector3));

	sub_indices = new unsigned short[vertex_count];
	memset(sub_indices, 0, vertex_count * sizeof(unsigned short));

	if (getUV)
	{
		uv_coords = new Ogre::Vector2[vertex_count];
		memset(uv_coords, 0, vertex_count * sizeof(Ogre::Vector2));
	}
	else uv_coords = NULL;

	indices = new unsigned long[index_count];
	memset(indices, 0, index_count * sizeof(unsigned long));

	added_shared = false;

	// Run through the submeshes again, adding the data into the arrays
	for (unsigned short subMeshIndex = 0; subMeshIndex < mesh->getNumSubMeshes(); ++subMeshIndex)
	{
		Ogre::SubMesh* submesh = mesh->getSubMesh(subMeshIndex);

		//----------------------------------------------------------------
		// GET VERTEXDATA
		//----------------------------------------------------------------

		//Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;
		Ogre::VertexData* vertex_data;

		//When there is animation:
		if (useSoftwareBlendingVertices)
			vertex_data = submesh->useSharedVertices ? entity->_getSkelAnimVertexData() : entity->getSubEntity(subMeshIndex)->_getSkelAnimVertexData();
		else
			vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;


		if ((!submesh->useSharedVertices) || (submesh->useSharedVertices && !added_shared))
		{
			if (submesh->useSharedVertices)
			{
				added_shared = true;
			}


			// There is _no_ baseVertexPointerToElement() which takes an Ogre::Real or a double
			//  as second argument. So make it float, to avoid trouble when Ogre::Real will
			//  be comiled/typedefed as double:
			//      Ogre::Real* pReal;
			float* pReal;
			unsigned char* vertex;

			// load vertex positions
			const Ogre::VertexElement* posElem =
				vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);

			if (posElem)
			{
				Ogre::HardwareVertexBufferSharedPtr vbuf =
					vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());

				vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

				for (size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
				{
					posElem->baseVertexPointerToElement(vertex, &pReal);

					Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);

					vertices[current_offset_v + j] = (orient * (pt * scale)) + position;
					sub_indices[current_offset_v + j] = subMeshIndex;

				}
				vbuf->unlock();
				next_offset_v += vertex_data->vertexCount;
			}


			// load texture coordinates
			if (getUV)
			{
				const Ogre::VertexElement* uvElem =
					vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_TEXTURE_COORDINATES);

				if (uvElem)
				{
					Ogre::HardwareVertexBufferSharedPtr tbuf =
						vertex_data->vertexBufferBinding->getBuffer(uvElem->getSource());

					vertex = static_cast<unsigned char*>(tbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

					for (size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += tbuf->getVertexSize())
					{
						uvElem->baseVertexPointerToElement(vertex, &pReal);
						uv_coords[current_offset_t + j] = Ogre::Vector2(pReal[0], pReal[1]);
					}
					tbuf->unlock();
					next_offset_t += vertex_data->vertexCount;
				}
			}
		}

		Ogre::IndexData* index_data = submesh->indexData;
		size_t numTris = index_data->indexCount / 3;
		Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;

		bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

		unsigned long*  pLong = static_cast<unsigned long*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
		unsigned short* pShort = reinterpret_cast<unsigned short*>(pLong);


		if (use32bitindexes)
		{
			for (size_t k = 0; k < numTris*3; ++k)
			{
				indices[index_offset++] = pLong[k] + current_offset_v;
			}
		}
		else
		{
			for (size_t k = 0; k < numTris*3; ++k)
			{
				indices[index_offset++] = static_cast<unsigned long>(pShort[k] + current_offset_v);
			}
		}

		ibuf->unlock();
		current_offset_v = next_offset_v;
		current_offset_t = next_offset_t;
	}

} 

//////////////////////////////////////////////////////////////////////////
void OgreUtil::GetBoneWorldPosition(Ogre::Entity* entity, Ogre::Bone* bone, Ogre::Vector3& worldPosition, Ogre::Quaternion& worldOrientation)
{
	worldPosition = bone->_getDerivedPosition();
	worldOrientation = bone->_getDerivedOrientation();

	// multiply with the parent derived transformation
	Ogre::Node* parentNode = entity->getParentNode();
	Ogre::SceneNode* parentSceneNode = entity->getParentSceneNode();
	
	while (parentNode != NULL)
	{
		if (parentNode != parentSceneNode)
		{
			// attached to a tag point
			Ogre::TagPoint* tagPoint = static_cast<Ogre::TagPoint*>(parentNode);
			
			worldPosition = tagPoint->_getFullLocalTransform() * worldPosition;
			worldOrientation = tagPoint->_getDerivedOrientation() * worldOrientation;
			
			parentNode = tagPoint->getParent();
		}
		else
		{
			// attached to a scene node
			worldPosition = parentNode->_getFullTransform() * worldPosition;
			worldOrientation = parentNode->_getDerivedOrientation() * worldOrientation;

			break;
		}
	}
}


} // namespace Wme
