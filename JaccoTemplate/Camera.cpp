#include "precomp.h"
#include "Camera.h"

MathUtil::mat4 Tmpl8::Camera::GetView()
{
   MathUtil::mat4 view;

   MathUtil::mat4 xRotMat = MathUtil::mat4::identity().rotatex(rotation.x);
   MathUtil::mat4 yRotMat = MathUtil::mat4::identity().rotatey(rotation.y);
   MathUtil::mat4 zRotMat = MathUtil::mat4::identity().rotatez(rotation.z);


   MathUtil::mat4 RotationMatrix = zRotMat  * yRotMat * xRotMat;
   MathUtil::mat4 translationMatrix = MathUtil::mat4::identity();
   translationMatrix.cell[3] = position.x;
   translationMatrix.cell[7] = position.y;
   translationMatrix.cell[11] = position.z;

   view = translationMatrix * RotationMatrix;
   return view;
}

MathUtil::mat4 Tmpl8::Camera::GetProjection()
{
    MathUtil::mat4 projection;

	projection = MathUtil::mat4::identity();

	float scale = 1.f / static_cast<float>(tanf(m_Fov * 0.5f * (MathUtil::PI / 180.f)));
	float OneOvernearminfar = 1.f / (m_Near - m_Far);
	projection[0] = scale/m_AspectRatio; projection[1] = 0.f;                    projection[2] = 0.f;											projection[3] = 0.f;
	projection[4] = 0.f;				 projection[5] = scale;					 projection[6] = 0.f;											projection[7] = 0.f;
	projection[8] = 0.f;				 projection[9] = 0.f;                    projection[10] = (m_Far + m_Near) * OneOvernearminfar;			projection[11] = -2.f * m_Far * m_Near * OneOvernearminfar;
	projection[12] = 0.f;				 projection[13] = 0.f;                   projection[14] = -1.f;											projection[15] = 0.f;

    return projection;
}
