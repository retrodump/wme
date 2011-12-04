// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeSerialize_H__
#define __WmeSerialize_H__

#include <boost/serialization/serialization.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#define SERIALIZE(class_name) \
	template void class_name::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, unsigned int);\
	template void class_name::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, unsigned int);



#endif // __WmeSerialize_H__