#include "precomp.h"
#include "Renderer.h"
#include "Surface.h"
#include "Texture.h"
#include "Camera.h"

#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <vector>

void Tmpl8::Renderer::FillShape(Vertex& vertexOne, Vertex& vertexTwo, Vertex& vertexThree)
{
	MathUtil::vec3 oneOverW = { vertexOne.position.w, vertexTwo.position.w, vertexThree.position.w };
	oneOverW = MathUtil::vec3(1.f) / oneOverW;
	

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
			MathUtil::vec3 BCcoordinate = currentBC;
			BCcoordinate = (BCcoordinate * oneOverW) / (BCcoordinate.dot(oneOverW));
			
			if (x >= ScreenWidth)
				break;

			MathUtil::vec4 position = vertexOne.position * BCcoordinate.x +
				vertexTwo.position * BCcoordinate.y +
				vertexThree.position * BCcoordinate.z;

			if (m_DepthBuffer[x + y * ScreenWidth] < position.z / position.w)
				continue;

			MathUtil::vec2 texcoord = vertexOne.texCoords * BCcoordinate.x +
				vertexTwo.texCoords * BCcoordinate.y +
				vertexThree.texCoords * BCcoordinate.z;

			//m_BitMap[x + y * ScreenWidth] = Vec3ToPixel(MathUtil::vec3(texcoord.x, texcoord.y, 0.f));
			//m_BitMap[x + y * ScreenWidth] = m_BoundTexture->SampleLinearRepeat(texcoord.x, texcoord.y);
			//m_BitMap[x + y * ScreenWidth] = m_BoundTexture->SampleNearestRepeat(texcoord.x, texcoord.y);
			m_BitMap[x + y * ScreenWidth] = Vec3ToPixel(BCcoordinate);
			m_DepthBuffer[x + y * ScreenWidth] = position.z / position.w;
			currentBC += BCstep;
		}
	}
}
bool Tmpl8::Renderer::ClipPlane(std::vector<Vertex>& vertices, std::vector<Vertex>& resultVertices, int axis)
{
	ClipComponent(vertices, axis, 1.0f, resultVertices);
	vertices.clear();

	if(resultVertices.empty())
		return false;

	ClipComponent(resultVertices, axis, -1.0f, vertices);
	resultVertices.clear();
	
	return !vertices.empty();
}
void Tmpl8::Renderer::ClipComponent(std::vector<Vertex>& vertices, int plane, float componentFactor, std::vector<Vertex>& resultVertices)
{
	Vertex previousVertex = vertices[vertices.size() - 1];
	float previousComponent =  previousVertex.position[plane] * componentFactor;
	bool previousInside = previousComponent <= previousVertex.position.w;

	for (int i = 0; i < vertices.size(); i++)
	{
		Vertex currentVertex = vertices[i];
		float currentComponent = currentVertex.position[plane] * componentFactor;
		bool currentInside = currentComponent <= currentVertex.position.w;

		//only one is true
		if(currentInside ^ previousInside)
		{
			float lerpAmt = (previousVertex.position.w - previousComponent) /
				((previousVertex.position.w - previousComponent) -
					(currentVertex.position.w - currentComponent));

			Vertex outPutVertex;
			outPutVertex.position = MathUtil::Lerp(previousVertex.position, currentVertex.position, { lerpAmt, lerpAmt, lerpAmt,lerpAmt });
			//outPutVertex.NDC = MathUtil::Lerp(outPutVertex.NDC, currentVertex.NDC, { lerpAmt, lerpAmt, lerpAmt,lerpAmt });
			outPutVertex.texCoords = MathUtil::Lerp(previousVertex.texCoords, currentVertex.texCoords, { lerpAmt, lerpAmt});
			
			resultVertices.push_back(outPutVertex);
		}
		if(currentInside)
		{
			resultVertices.push_back(currentVertex);
		}
		previousVertex = currentVertex;
		previousComponent = currentComponent;
		previousInside = currentInside;
	}
}

void Tmpl8::Renderer::Draw(const MathUtil::mat4& modelMatrix, const Vertex* aVertices, const Triangle* aTriangles, int numVertices, int numTriangles)
{
	std::vector<Vertex> outVertices;
	//create a new triangle list that has the transformed triangles
	MathUtil::mat4 invertedView = m_CameraRef->GetView();
	invertedView.invert();
	MathUtil::mat4 MVP = m_CameraRef->GetProjection() * invertedView * modelMatrix;

	for (int i = 0; i < numTriangles; i++)
	{
		Vertex vertices[3];
		vertices[0] = aVertices[aTriangles[i].indices[0]];
		vertices[1] = aVertices[aTriangles[i].indices[1]];
		vertices[2] = aVertices[aTriangles[i].indices[2]];

		//transform from 
		for (int j = 0; j < 3; j++)
		{
			vertices[j].position = vertices[j].position * (MVP);
		}
		
		outVertices.push_back(vertices[0]);
		outVertices.push_back(vertices[1]);
		outVertices.push_back(vertices[2]);
	}
	//draw the transformed triangle list
	for(int i = 0; i < outVertices.size(); i+=3)
	{
		std::vector<Vertex> clippedVertices;
		std::vector<Vertex> tempList;
		clippedVertices.push_back(outVertices[i + 0]);
		clippedVertices.push_back(outVertices[i + 1]);
		clippedVertices.push_back(outVertices[i + 2]);

		//clipping
		if(ClipPlane(clippedVertices, tempList, 0) &&
			ClipPlane(clippedVertices, tempList, 1) &&
				ClipPlane(clippedVertices, tempList, 2))
		{

			for (int j = 0; j < clippedVertices.size(); j++)
			{

				clippedVertices[j].NDC.x = clippedVertices[j].position.x / clippedVertices[j].position.w;
				clippedVertices[j].NDC.y = clippedVertices[j].position.y / clippedVertices[j].position.w;
				clippedVertices[j].NDC.z = clippedVertices[j].position.z / clippedVertices[j].position.w;
				clippedVertices[j].NDC.w = clippedVertices[j].position.w / clippedVertices[j].position.w;
			}


			Vertex initialVertex = clippedVertices[0];
			for (int j = 1; j < clippedVertices.size() - 1; j++)
			{
				DrawTriangle(initialVertex, clippedVertices[j], clippedVertices[j + 1]);
			}
		}
	/*	for (int j = 0; j < 3; j++)
		{
			shouldAdd = shouldAdd && vertexPos[j].x > -1.f;
			shouldAdd = shouldAdd && vertexPos[j].x < 1.f;

			shouldAdd = shouldAdd && vertexPos[j].y > -1.f;
			shouldAdd = shouldAdd && vertexPos[j].y < 1.f;

			shouldAdd = shouldAdd && vertexPos[j].z > -1.f;
			shouldAdd = shouldAdd && vertexPos[j].z < 1.f;
		}*/

		
	}
}

void Tmpl8::Renderer::ScanLine(MathUtil::vec3 aStart, MathUtil::vec3 aEnd, MathUtil::vec3 aBCstart, MathUtil::vec3 aBCend, bool aRightHanded)
{
	Texture* i = m_BoundTexture;

	int yStart = static_cast<int>(ceilf(aStart.y));
	//if it is out of screen
	if (yStart > ScreenHeight)
		return;
	
	int yEnd = static_cast<int>(ceilf(aEnd.y));
	if (yEnd < 0)
		return;

	float yDist = aEnd.y - aStart.y;
	float xDist = aEnd.x - aStart.x;

	if (yDist <= 0.f)
		return;
	//
	int* buffer = aRightHanded ? m_BeginBuffer : m_EndBuffer;
	MathUtil::vec3* BCbuffer = aRightHanded ? m_BarycentricStart : m_BarycentricEnd;

	float xStep = xDist / yDist;
	MathUtil::vec3 BCstep = (aBCend - aBCstart) / yDist;

	float yPreStep = static_cast<float>(yStart) - (aStart.y);
	float currentX = aStart.x + yPreStep * xStep;

	MathUtil::vec3 currentBC = aBCstart;
	
	for(int y = yStart; y < yEnd; y++)
	{
		buffer[y] = static_cast<int>(round(currentX));
		BCbuffer[y] = currentBC;

		currentBC += BCstep;
		currentX += xStep;
	}
}

void Tmpl8::Renderer::DrawTriangle(Vertex vertexOne, Vertex vertexTwo, Vertex vertexThree)
{
	//printf("begin draw\n");
	//printf("%i\n",m_BoundTexture);


	FlushBuffers();
	

	MathUtil::mat4 m_NDCtoScreenM;

	m_NDCtoScreenM.cell[0] = -static_cast<float>(ScreenWidth) / 2.f; m_NDCtoScreenM.cell[1] = 0.f; m_NDCtoScreenM.cell[2] = 0.f; m_NDCtoScreenM.cell[3] = static_cast<float>(ScreenWidth) / 2.f;
	m_NDCtoScreenM.cell[4] = 0.f; m_NDCtoScreenM.cell[5] = static_cast<float>(ScreenHeight) / 2.f; m_NDCtoScreenM.cell[6] = 0.f; m_NDCtoScreenM.cell[7] = static_cast<float>(ScreenHeight) / 2.f;
	m_NDCtoScreenM.cell[8] = 0.f; m_NDCtoScreenM.cell[9] = 0.f; m_NDCtoScreenM.cell[10] = 1.f; m_NDCtoScreenM.cell[11] = 0.f;
	m_NDCtoScreenM.cell[12] = 0.f; m_NDCtoScreenM.cell[13] = 0.f; m_NDCtoScreenM.cell[14] = 0.f; m_NDCtoScreenM.cell[15] = 1.f;
	//transform from ndc to screenspace
	
	vertexOne.NDC = vertexOne.NDC * m_NDCtoScreenM;
	vertexTwo.NDC = vertexTwo.NDC * m_NDCtoScreenM;
	vertexThree.NDC = vertexThree.NDC * m_NDCtoScreenM;


	MathUtil::vec3 BCCoordinates[3] = { MathUtil::vec3(1,0,0) ,MathUtil::vec3(0,1,0) ,  MathUtil::vec3(0,0,1) };

	//order the points
	if (vertexOne.NDC.y > vertexTwo.NDC.y)
	{
		//std::swap(BCCoordinates[0], BCCoordinates[1]);
		std::swap(vertexOne, vertexTwo);
	}
	if(vertexTwo.NDC.y > vertexThree.NDC.y)
	{
		//std::swap(BCCoordinates[1], BCCoordinates[2]);
		std::swap(vertexTwo, vertexThree);

	}
	if (vertexOne.NDC.y > vertexTwo.NDC.y)
	{
		//std::swap(BCCoordinates[0], BCCoordinates[1]);
		std::swap(vertexOne, vertexTwo);
	}

	MathUtil::vec2 oneTo2 = MathUtil::vec2(vertexOne.NDC.x, vertexOne.NDC.y) -
		MathUtil::vec2(vertexTwo.NDC.x, vertexTwo.NDC.y);

	MathUtil::vec2 oneTo3 = MathUtil::vec2(vertexOne.NDC.x, vertexOne.NDC.y) -
		MathUtil::vec2(vertexThree.NDC.x, vertexThree.NDC.y);

	bool handedness = !(oneTo2.cross(oneTo3) < 0.f);


	ScanLine(vertexOne.NDC, vertexThree.NDC, BCCoordinates[0], BCCoordinates[2], handedness);
	ScanLine(vertexOne.NDC, vertexTwo.NDC, BCCoordinates[0], BCCoordinates[1], !handedness);
	ScanLine(vertexTwo.NDC, vertexThree.NDC, BCCoordinates[1], BCCoordinates[2], !handedness);
	
	

	FillShape(vertexOne, vertexTwo, vertexThree);
}
void Tmpl8::Renderer::Clear()
{
	for(int i = 0; i < ScreenWidth * ScreenHeight; i++)
	{
		m_BitMap[i] = 0x000000000;
		m_DepthBuffer[i] = 1.f;
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
void Tmpl8::Renderer::BindTexture(Texture* texture)
{
	m_BoundTexture = texture;
}
void Tmpl8::Renderer::FlushBuffers()
{
	for (int y = 0; y < ScreenHeight; y++)
	{
		m_BeginBuffer[y] = -1;
		m_EndBuffer[y] = -1;
		
	}
	
}
