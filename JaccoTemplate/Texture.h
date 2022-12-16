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
		~Texture() { delete[] m_Pixels; }

		Pixel SampleNearestRepeat(float x, float y); 
		Pixel SampleLinearRepeat(float x, float y);
	private:
		int m_Width;
		int m_Height;

		Pixel* m_Pixels;
	};
}

