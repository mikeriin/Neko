#ifndef CAMERA_H
#define CAMERA_H


#include "Neko.h"


#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
using vec3 = glm::vec3;
using vec2 = glm::vec2;
using mat4 = glm::mat4;


struct CameraSettings
{
	vec3 Position{ 0.0, 0.0, 0.0 };
	vec3 Front{ 0.0, 0.0, -1.0 };
	vec3 Right{ 1.0, 0.0, 0.0 };
	vec3 Up{ 0.0, 1.0, 0.0 };

	f32 FOV{ 45.0f };
	f32 AspectRatio{ 16.0f / 9.0f };
	f32 NearPlane{ 0.01f };
	f32 FarPlane{ 100.0f };

	f32 Speed{ 1.0f };
	f32 Yaw{ 0.0f };
	f32 Pitch{ 0.0f };
	f32 SensitivityX{ 1.0f };
	f32 sensitivityY{ 0.85f };
};


class EventHandler;

class Camera
{
private:
	// view
	vec3 m_Position;
	vec3 m_Front;
	vec3 m_Right;
	vec3 m_Up;
	
	// projection
	f32 m_FOV;
	f32 m_AspectRatio;
	f32 m_NearPlane;
	f32 m_FarPlane;

	// controls
	f32 m_Speed;
	f32 m_Yaw;
	f32 m_Pitch;
	f32 m_SensitivityX;
	f32 m_SensitivityY;


public:
	Camera();
	Camera(const CameraSettings& settings);
	~Camera() = default;

	mat4 GetProjectionMatrix() const;
	mat4 GetViewMatrix() const;
	inline void SetAspectRatio(f32 aspect) { m_AspectRatio = aspect; }

	void Update(EventHandler& events, f64 dt);
};


#endif // !CAMERA_H
