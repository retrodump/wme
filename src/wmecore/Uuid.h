// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeUuid_H__
#define __WmeUuid_H__

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace Wme
{
	class WmeDllExport Uuid
	{
	public:
		enum UuidType
		{
			UUID_NEW
		};

		Uuid();
		Uuid(const Uuid& id);
		Uuid(const byte buffer[16]);
		Uuid(const WideString& str);
		Uuid(UuidType Type);
		virtual ~Uuid();

		Uuid& operator= (const Uuid& id);
		bool operator== (const Uuid& id);
		bool operator!= (const Uuid& id);

		void Clone(const Uuid& id);
		int Compare(const Uuid& id);

		void Generate();
		void Clear();
		bool IsEmpty();
		WideString ToString() const;
		bool ParseString(const WideString& str);
		void FromBinary(const byte buffer[16]);
		void GetBinary(byte buffer[16]) const;

	private:
		boost::uuids::uuid m_Uuid;
	};
}

#endif // __WmeUuid_H__
