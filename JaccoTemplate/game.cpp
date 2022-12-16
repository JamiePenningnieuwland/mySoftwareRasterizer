#include "precomp.h"
#include "game.h"
#include "Renderer.h"
#include "Camera.h"
#include "Model.h"
#include "vec.h"
#include <string>

namespace Tmpl8
{

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
		camera =  new Camera(90.f,static_cast<float>(ScreenWidth) / ScreenHeight, .1f, 10.f);
		camera->position = {0.f, 0.f, -5.f};

		TheRenderer = new Renderer(camera);
		model = new Model("Resources/Models/DamagedHelmet/glTF/DamagedHelmet.gltf");

	}
	void Game::Shutdown()
	{
		TheRenderer->Clear();
		TheRenderer->FlushBuffers();
		delete model;
		delete TheRenderer;
	}
	void Game::UpdateCamera(float deltaTime)
	{
		MathUtil::vec3 rotation(0.f);
		float sens = 0.8f;
		float movementSpeed = 2.f;
		//wsda
		if (keys[82])
		{
			rotation.x += 1.f;
		
		}
		if (keys[81])
		{
			rotation.x -= 1.f;
		
		}
		if (keys[80])
		{
			rotation.y += 1.f;
		
		}
		if (keys[79])
		{
			rotation.y -= 1.f;
			
		}
		// q and e
		if (keys[8])
		{
			rotation.z += 1.f;
		}
		if (keys[20])
		{
			rotation.z -= 1.f;
		}
		rotation *= deltaTime * sens;
		MathUtil::vec3 camRotation = camera->rotation;
		camRotation += rotation;
		camera->rotation = camRotation;

		MathUtil::vec3 movement(0.f);
		if (keys[4])
		{
			movement.x += 1.f;
		
		}
		if (keys[7])
		{
			movement.x -= 1.f;
		
		}
		if (keys[225])
		{
			movement.y += 1.f;
			
		}
		if (keys[44])
		{
			movement.y -= 1.f;

		}
		// q and e
		if (keys[22])
		{
			movement.z -= 1.f;
		
		}
		if (keys[26])
		{
			movement.z += 1.f;

		}
		if (movement.sqrLentgh() > 0.1f)
		{
			movement.normalize();
			MathUtil::mat4 view = camera->GetView();
			view.invert();
			movement = movement * view;
			MathUtil::vec3 camPosition = camera->position;
			camPosition += movement * deltaTime * movementSpeed;
			camera->position = camPosition;

		}
		
	}
	void Game::Tick(float deltaTime)
	{
		UpdateCamera(deltaTime);
	/*	Triangle triangle;
		triangle.vertex1 = { {100,100,0,0} };
		triangle.vertex2 = { { 50,200,0,0 } };
		triangle.vertex3 = { {100, 300, 0, 0}};*/
		
		//TheRenderer->m_CameraRef = camera;

		screen->Clear(0xFFFFFF);

		TheRenderer->Clear();
		
		model->Draw(MathUtil::mat4().identity(), TheRenderer);

		TheRenderer->CopyToSurface(screen);

	};

}