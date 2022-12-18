#pragma once
#include "surface.h"

#include <string>
namespace Tmpl8
{
	class Texture
	{
	public:
		Texture(int width, int height);
		Texture(const std::string& filepath);
		~Texture();

		Pixel SampleNearestRepeat(float x, float y); 
		Pixel SampleLinearRepeat(float x, float y);
	private:
		int m_Width = 0;
		int m_Height = 0;

		Pixel* m_Pixels{ nullptr };
	};
}

