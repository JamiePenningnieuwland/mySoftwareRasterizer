#include "precomp.h"
#include "Texture.h"
#include "vec.h"

#include <stb_image.h>

Tmpl8::Texture::Texture(int width, int height)
	:m_Width(width), m_Height(height)
{
	m_Pixels = new Tmpl8::Pixel[m_Width * m_Height];
	for(int y = 0; y < m_Height; y++)
	{
		for(int x = 0; x < m_Width; x++)
		{
			byte r = static_cast<byte>(rand()) % 255;
			byte g = static_cast<byte>(rand()) % 255;
			byte b = static_cast<byte>(rand()) % 255;
		

			m_Pixels[x + y * m_Width] = (r << (8 * 2)) | (g << (8 * 1)) | (b << (8 * 0));
		}
	}
}

Tmpl8::Texture::Texture(const std::string& filepath)
{
	unsigned char* data;
	int numchannel = 4;
	data = stbi_load(filepath.c_str(), &m_Width, &m_Height, &numchannel, 4);

	m_Pixels = new Tmpl8::Pixel[m_Width * m_Height];
	memcpy(m_Pixels, data, m_Width * m_Height * 4);

	stbi_image_free(data);
}

Tmpl8::Pixel Tmpl8::Texture::SampleNearestRepeat(float x, float y)
{
	//always between 0-1
	x = abs(x) * static_cast<float>(m_Width);


	y = abs(y) * static_cast<float>(m_Height);

	int pixelX = static_cast<int>(x) % m_Width;
	int pixelY = static_cast<int>(y) % m_Height;

	return m_Pixels[pixelX + pixelY * m_Width];
}

Tmpl8::Pixel Tmpl8::Texture::SampleLinearRepeat(float x, float y)
{
	//always between 0-1
	x = abs(x);
	x = x - (long)(x);

	y = abs(y);
	y = y - (long)(y);

	float pixelX = x * (m_Width);
	float pixelY = y * (m_Height);

	float xFract = pixelX - (long)(pixelX);
	float yFract = pixelY - (long)(pixelY);

	int leftX = static_cast<int>(floorf(pixelX));
	int rightX = static_cast<int>(ceilf(pixelX)) % m_Width;

	int topY = static_cast<int>(floorf(pixelY));
	int bottomY = static_cast<int>(ceilf(pixelY)) % m_Height;

	
	MathUtil::vec3 colorTopLeft = PixelToVec3(m_Pixels[leftX + topY * m_Width]);
	MathUtil::vec3 colorTopRight = PixelToVec3(m_Pixels[rightX + topY * m_Width]);

	MathUtil::vec3 colorBottomLeft = PixelToVec3(m_Pixels[leftX + bottomY * m_Width]);
	MathUtil::vec3 colorBottomRight = PixelToVec3(m_Pixels[rightX + bottomY * m_Width]);

	MathUtil::vec3 top = colorTopLeft * (1.f - xFract) + colorTopRight * xFract ;
	MathUtil::vec3 bottom = colorBottomLeft * (1.f - xFract) + colorBottomRight * xFract ;

	MathUtil::vec3 finalColor = top * (1.f - yFract) + bottom * yFract;


	return Vec3ToPixel(finalColor);
}
