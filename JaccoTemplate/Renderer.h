#pragma once

#include "template.h"
#include <vec.h>


namespace Tmpl8
{
	class Surface;
	class Renderer
	{
	public:
		Renderer() {}
		void FlushBuffers();
		void FillShape(Triangle& triangle);
		void temp();
		void DrawTriangle(Triangle aTriangle);
		void Clear();
		void CopyToSurface(Surface* aSurface);
	private:
		void ScanLine(MathUtil::vec3 aStart, MathUtil::vec3 aEnd, MathUtil::vec3 aBCstart, MathUtil::vec3 aBCend, bool aRightHanded);
		unsigned int m_BitMap[ScreenWidth * ScreenHeight * 4];
		int m_BeginBuffer[ScreenHeight];
		int m_EndBuffer[ScreenHeight];

		MathUtil::vec3 m_BarycentricStart[ScreenHeight];
		MathUtil::vec3 m_BarycentricEnd[ScreenHeight];
	};
};