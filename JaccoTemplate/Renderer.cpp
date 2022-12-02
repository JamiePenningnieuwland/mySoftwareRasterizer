#include "precomp.h"
#include "Renderer.h"
#include "Surface.h"
#include <stdio.h>
#include <string.h>
#include <algorithm>

Tmpl8::Pixel Vec3ToPixel(MathUtil::vec3& color)
{

	float R = MathUtil::Clamp(color.r,  0.f, 1.f);
	float G = MathUtil::Clamp(color.g,  0.f, 1.f);
	float B = MathUtil::Clamp(color.b,  0.f, 1.f);

	//bit shift into integer 
	byte newR = static_cast<byte>(R * 255.f);
	byte newG = static_cast<byte>(G * 255.f);
	byte newB = static_cast<byte>(B * 255.f);
	int newColor = (newR << (8 * 2)) | (newG << (8 * 1)) | (newB << (8 * 0));
	return newColor;
}

void Tmpl8::Renderer::FillShape(Triangle& aTriangle)
{
	for (int y = 0; y < ScreenHeight; y++)
	{
		int beginX = m_BeginBuffer[y];
		int endX = m_EndBuffer[y];

		MathUtil::vec3 beginBC = m_BarycentricStart[y];
		MathUtil::vec3 endBC = m_BarycentricEnd[y];

		float xDist = static_cast<float>(endX - beginX);

		MathUtil::vec3 BCstep = (endBC - beginBC) / xDist;
		MathUtil::vec3 currentBC = beginBC;

		if (beginX < 0 || endX < 0)
			continue;
		for (int x = beginX; x <= endX; x++)
		{
			if (x >= ScreenWidth)
				break;

			MathUtil::vec2 texcoord = aTriangle.vertex1.texCoords * currentBC.x +
				aTriangle.vertex2.texCoords * currentBC.y +
				aTriangle.vertex3.texCoords * currentBC.z;

			m_BitMap[x + y * ScreenWidth] = Vec3ToPixel(MathUtil::vec3(texcoord.x, texcoord.y, 0.f));
			currentBC += BCstep;
		}
	}
}
void Tmpl8::Renderer::temp()
{
	for (int i = 50; i < 100; i++)
	{
		m_BeginBuffer[i] = i;
		m_EndBuffer[i] = 100;
	}
}
void Tmpl8::Renderer::ScanLine(MathUtil::vec3 aStart, MathUtil::vec3 aEnd, MathUtil::vec3 aBCstart, MathUtil::vec3 aBCend, bool aRightHanded)
{
	int yStart = aStart.y;
	int yEnd = aEnd.y;

	float yDist = aEnd.y - aStart.y;
	float xDist = aEnd.x - aStart.x;

	float xStep = xDist / yDist;
	MathUtil::vec3 BCstep = (aBCend - aBCstart) / yDist;
	int* buffer = aRightHanded ? m_BeginBuffer : m_EndBuffer;
	MathUtil::vec3* BCbuffer = aRightHanded ? m_BarycentricStart : m_BarycentricEnd;

	float currentX = aStart.x;
	MathUtil::vec3 currentBC = aBCstart;
	for(int y = yStart; y < yEnd; y++)
	{
		buffer[y] = currentX;
		BCbuffer[y] = currentBC;

		currentX += xStep;
		currentBC += BCstep;
	}
}
void Tmpl8::Renderer::DrawTriangle(Triangle aTriangle)
{
	FlushBuffers();

	//order the points
	if (aTriangle.vertices[0].position.y > aTriangle.vertices[1].position.y)
	{
		std::swap(aTriangle.vertex1, aTriangle.vertex2);
	}
	if(aTriangle.vertices[1].position.y > aTriangle.vertices[2].position.y)
	{
		std::swap(aTriangle.vertex2, aTriangle.vertex3);

	}
	if (aTriangle.vertices[0].position.y > aTriangle.vertices[1].position.y)
	{
		std::swap(aTriangle.vertex1, aTriangle.vertex2);
	}

	int startY = aTriangle.vertices[0].position.y;
	int endY = aTriangle.vertices[2].position.y;
	MathUtil::vec2 oneTo2 = MathUtil::vec2(aTriangle.vertices[0].position.x, aTriangle.vertices[0].position.y) - 
		MathUtil::vec2(aTriangle.vertices[1].position.x, aTriangle.vertices[1].position.y);

	MathUtil::vec2 oneTo3 = MathUtil::vec2(aTriangle.vertices[0].position.x, aTriangle.vertices[0].position.y) - 
		MathUtil::vec2(aTriangle.vertices[2].position.x, aTriangle.vertices[2].position.y);

	bool handedness = !(oneTo2.cross(oneTo3) < 0.f);

	ScanLine(aTriangle.vertex1.position, aTriangle.vertex3.position, MathUtil::vec3(1,0,0), MathUtil::vec3(0,0,1), handedness);
	ScanLine(aTriangle.vertex1.position, aTriangle.vertex2.position, MathUtil::vec3(1,0,0), MathUtil::vec3(0,1,0), !handedness);
	ScanLine(aTriangle.vertex2.position, aTriangle.vertex3.position, MathUtil::vec3(0,1,0), MathUtil::vec3(0,0,1), !handedness);

	FillShape(aTriangle);
}
void Tmpl8::Renderer::Clear()
{
	for(int i = 0; i < ScreenWidth * ScreenHeight * 4; i++)
	{
		m_BitMap[i] = 0x000000000;
	}
}
void Tmpl8::Renderer::CopyToSurface(Surface* aSurface)
{
	if (ScreenWidth != aSurface->GetWidth() || ScreenHeight != aSurface->GetHeight())
	{
		printf("not a suitable surface");
		return;
	}
	memcpy(aSurface->GetBuffer(), m_BitMap, (static_cast<size_t>(ScreenHeight * ScreenWidth) * 4));
}
void Tmpl8::Renderer::FlushBuffers()
{
	for (int y = 0; y < ScreenHeight; y++)
	{
		m_BeginBuffer[y] = -1;
		m_EndBuffer[y] = -1;
		
	}
	
}
