// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeException_H__
#define __WmeException_H__


namespace Wme
{
	class WmeDllExport Exception : public std::exception
	{
	public:
		Exception();
		Exception(const WideString& description);
		~Exception() throw();

		const WideString& GetDescription() const { return m_Description; }

	private:
		WideString m_Description;
	};
}

#endif // __WmeException_H__
