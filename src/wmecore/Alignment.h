// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeAlignment_H__
#define __WmeAlignment_H__


namespace Wme
{
	class WmeDllExport Alignment
	{
	public:
		enum Vertical
		{
			VA_TOP,
			VA_CENTER,
			VA_BOTTOM
		};

		enum Horizontal
		{
			HA_LEFT,
			HA_CENTER,
			HA_RIGHT
		};

		Alignment();
		Alignment(Horizontal horizontal, Vertical vertical);
		~Alignment();

		Alignment(const Alignment& a);
		Alignment& operator= (const Alignment& a);
		bool operator!= (const Alignment& a) const;
		bool operator== (const Alignment& a) const;

		void Clone(const Alignment& a);

		Vertical GetVertical() const { return m_Vertical; }
		void SetVertical(Vertical val) { m_Vertical = val; }

		Horizontal GetHorizontal() const { return m_Horizontal; }
		void SetHorizontal(Horizontal val) { m_Horizontal = val; }

		void SetAlignment(Horizontal horizontal, Vertical vertical);

		bool LoadFromXml(TiXmlElement* node);
		bool SaveToXml(TiXmlElement* node);



	private:
		Vertical m_Vertical;
		Horizontal m_Horizontal;
	};
}

#endif // __WmeAlignment_H__
