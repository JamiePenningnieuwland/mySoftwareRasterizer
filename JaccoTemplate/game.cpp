#include "precomp.h"
#include "Renderer.h"
#include "vec.h"
#include "game.h"


namespace Tmpl8
{

	void FillShape(int* begin_buffer, int* end_buffer, Surface* aScreen)
	{
		for(int y = 0; y < aScreen->GetHeight(); y++)
		{
			int beginX = begin_buffer[y];
			int endX = end_buffer[y];

			if (beginX < 0 || endX < 0)
				continue;
			for(int x = beginX; x <= endX; x++)
			{
				if (x >= aScreen->GetWidth())
					break;
				
				aScreen->Plot(x, y, 0xFF00FF);
			}
		}
	}
	Renderer* TheRenderer;
	//keys
	void Game::KeyDown(int key)
	{
	
		keys[key] = true;
	}
	void Game::KeyUp(int key)
	{
		keys[key] = false;
	}

	void Game::Init()
	{
		TheRenderer = new Renderer();
		

	}
	void Game::Shutdown()
	{
		TheRenderer->Clear();
		TheRenderer->FlushBuffers();
		delete TheRenderer;
	}

	void Game::Tick(float deltaTime)
	{
	/*	Triangle triangle;
		triangle.vertex1 = { {100,100,0,0} };
		triangle.vertex2 = { { 50,200,0,0 } };
		triangle.vertex3 = { {100, 300, 0, 0}};*/
		static float t = 0;
		t += deltaTime;
		float pos = 512 / 3 * 2 + sin(t/2.) * 50.;


		Triangle triangle2;
		triangle2.vertex1 = { {512 / 3*2,512 / 3,0,0} ,{1,0} };
		triangle2.vertex2 = { { 512 / 3,512 / 3 * 2,0,0 } , {0,1} };
		triangle2.vertex3 = { {512 / 3 * 2,pos, 0, 0} ,{1,1} };
		Triangle triangle1;
		triangle1.vertex1 = { {512 / 3*2,512 / 3,0,0} ,{1,0} };
		triangle1.vertex2 = { { 512 / 3,512 / 3 * 2,0,0 } , {0,1} };
		triangle1.vertex3 = { {512 / 3, 512 / 3, 0, 0} ,{0,0} };
		screen->Clear(0xFFFFFF);

		TheRenderer->Clear();
		TheRenderer->DrawTriangle(triangle2);
		TheRenderer->DrawTriangle(triangle1);
	/*	TheRenderer->DrawTriangle(triangle);*/
		TheRenderer->CopyToSurface(screen);

	};

}