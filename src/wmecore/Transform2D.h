// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeTransform2D_H__
#define __WmeTransform2D_H__


namespace Wme
{
	class WmeDllExport Transform2D
	{
	public:
		Transform2D();
		explicit Transform2D(const Ogre::Matrix4& matrix);
		virtual ~Transform2D();

		bool operator==(const Transform2D& trans) const;
		bool operator!=(const Transform2D& trans) const;

		Transform2D& operator*=(const Transform2D& trans);
		Transform2D operator*(const Transform2D& trans) const;

		Transform2D& operator=(const Transform2D& trans);


		Transform2D& Translate(float x, float y);
		Transform2D& Rotate(float degrees);
		Transform2D& Scale(float x, float y);

		Transform2D& Reset();

		const Ogre::Matrix4& GetMatrix() const { return m_Matrix; }
		void SetMatrix(const Ogre::Matrix4& matrix) { m_Matrix = matrix; }

	private:
		Ogre::Matrix4 m_Matrix;
	};
}

#endif // __WmeTransform2D_H__
