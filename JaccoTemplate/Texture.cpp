#include "precomp.h"
#include "Texture.h"
#include "vec.h"
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

Tmpl8::Pixel Tmpl8::Texture::SampleNearestRepeat(float x, float y)
{
	//always between 0-1
	x = abs(x);
	x = x - (long)(x);

	y = abs(y);
	y = y - (long)(y);

	int pixelX = x * (m_Width - 1);
	int pixelY = y * (m_Height - 1);

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

	float xDiff = pixelX - (long)(pixelX);
	float yDiff = pixelY - (long)(pixelY);

	int leftX = floor(pixelX);
	int rightX = (int)(ceil(pixelX)) % m_Height;

	int topY = floor(pixelY);
	int bottomY = (int)(ceil(pixelY)) % m_Height;

	
	MathUtil::vec3 colorTopLeft = PixelToVec3(m_Pixels[leftX + topY * m_Width]);
	MathUtil::vec3 colorTopRight = PixelToVec3(m_Pixels[rightX + topY * m_Width]);

	MathUtil::vec3 colorBottomLeft = PixelToVec3(m_Pixels[leftX + bottomY * m_Width]);
	MathUtil::vec3 colorBottomRight = PixelToVec3(m_Pixels[rightX + bottomY * m_Width]);

	MathUtil::vec3 top = colorTopLeft * (1.f - xDiff) + colorTopRight * xDiff ;
	MathUtil::vec3 bottom = colorBottomLeft * (1.f - xDiff) + colorBottomRight * xDiff ;

	MathUtil::vec3 finalColor = top * (1.f - yDiff) + bottom * yDiff;


	return Vec3ToPixel(finalColor);
}
