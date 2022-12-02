#pragma once

#include <string>
namespace Tmpl8 {

class Surface;
class Game
{
public:

	void SetTarget( Surface* surface ) { screen = surface; }
	
	void Init();
	void Shutdown();
	void Tick( float deltaTime );

	//input
	void MouseUp(int) {  }
	void MouseDown(int) {  }
	void MouseMove(int , int ) { }
	void KeyUp(int key);
	void KeyDown(int key);

private:

	bool rightKey = false;
	bool leftKey = false;
	bool keys[400];

private:
	Surface* screen{ nullptr };

};

}; 