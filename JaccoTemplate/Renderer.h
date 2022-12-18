#pragma once

#include "template.h"
#include <vec.h>


namespace Tmpl8
{
	class Surface;
	class Camera;
	class Texture;
	class Renderer
	{
	public:
		Renderer(Camera* camera): m_CameraRef(camera)
		{
			
		}
		void FlushBuffers();
		void FillShape(Vertex& vertexOne, Vertex& vertexTwo, Vertex& vertexThree);
		void Draw(const MathUtil::mat4& modelMatrix, const Vertex* vertices, const Triangle* triangles, int numVertices, int numTriangles);
		
		void DrawTriangle(Vertex vertexOne, Vertex vertexTwo, Vertex vertexThree);
		void Clear();
		void CopyToSurface(Surface* aSurface);
		void BindTexture(Texture* texture);
		
		Camera* m_CameraRef{nullptr};
	private:
		void ScanLine(MathUtil::vec3 aStart, MathUtil::vec3 aEnd, MathUtil::vec3 aBCstart, MathUtil::vec3 aBCend, bool aRightHanded);
		unsigned int m_BitMap[ScreenWidth * ScreenHeight];
		int m_BeginBuffer[ScreenHeight];
		int m_EndBuffer[ScreenHeight];
		float m_DepthBuffer[ScreenWidth * ScreenHeight];

		MathUtil::vec3 m_BarycentricStart[ScreenHeight];
		MathUtil::vec3 m_BarycentricEnd[ScreenHeight];
		Texture* m_BoundTexture{nullptr};
	};
};