#pragma once
#include "vec.h"

namespace Tmpl8
{
	class Camera
	{
	public:
		Camera() = default;
		Camera(float fov, float aspectRatio, float near, float far) : m_Fov(fov), m_AspectRatio(aspectRatio), m_Near(near), m_Far(far) {};
		~Camera()
		{
			int i = 0;
		}
		MathUtil::mat4 GetView();
		MathUtil::mat4 GetProjection();

		MathUtil::vec3 position;
		MathUtil::vec3 rotation;
	private:
		float m_Fov;
		float m_Far;
		float m_Near;
		float m_AspectRatio;
	};

}
