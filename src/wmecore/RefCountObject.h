// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeRefCountObject_H__
#define __WmeRefCountObject_H__


namespace Wme
{
	class WmeDllExport RefCountObject
	{
	// hide unwanted operations
	protected:
		RefCountObject(const RefCountObject&) {};
		virtual ~RefCountObject() {};
	private:
		RefCountObject& operator=(const RefCountObject&);

	public:
		RefCountObject() { m_RefCount = 0; }

		void AddRef() { m_RefCount++; }
		
		int ReleaseRef()
		{
			m_RefCount--;
			if (m_RefCount == 0)
			{
				delete this;
				return 0;
			}
			else return m_RefCount;
		}
		
		int GetRefCount() { return m_RefCount; }

	private:
		int m_RefCount;
	};
}

#endif // __WmeRefCountObject_H__
