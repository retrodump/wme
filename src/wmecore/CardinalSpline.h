// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeCardinalSpline_H__
#define __WmeCardinalSpline_H__


namespace Wme
{
	// Ogre::SimpleSpline (Catmull-Rom spline) with added tension (Cardinal spline)

	class WmeDllExport CardinalSpline
    {
    public:
        CardinalSpline();
        ~CardinalSpline();

		void AddPoint(const Ogre::Vector3& p);

        const Ogre::Vector3& GetPoint(unsigned short index) const;

        unsigned short GetNumPoints() const;

        void Clear();

		void UpdatePoint(unsigned short index, const Ogre::Vector3& value);

        Ogre::Vector3 Interpolate(Ogre::Real t) const;

        Ogre::Vector3 Interpolate(unsigned int fromIndex, Ogre::Real t) const;

        void SetAutoCalculate(bool autoCalc);

        void RecalcTangents();

		void SetTension(Ogre::Real tension) { m_Tension = tension; }
		Ogre::Real GetTension() { return m_Tension; }

    protected:
        bool m_AutoCalc;

		std::vector<Ogre::Vector3> m_Points;
        std::vector<Ogre::Vector3> m_Tangents;

		Ogre::Matrix4 m_Coeffs;

		// 1.0 = straight lines, 0.0 = Catmull-Rom spline
		Ogre::Real m_Tension;
    };
}

#endif // __WmeCardinalSpline_H__
