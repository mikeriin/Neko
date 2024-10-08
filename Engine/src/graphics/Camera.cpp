#include "graphics/Camera.h"


#include <algorithm>

#include "core/Event.h"



Camera::Camera()
	: m_Position(vec3{ 0.0 })
	, m_Front(vec3{ 0.0, 0.0, -1.0 })
	, m_Right(vec3{ 1.0, 0.0, 0.0 })
	, m_Up(vec3{ 0.0, 1.0, 0.0 })
	, m_FOV(glm::radians(45.0f))
	, m_AspectRatio(16.0f / 9.0f)
	, m_NearPlane(0.01f)
	, m_FarPlane(100.0f)
	, m_Speed(1.0f)
	, m_Yaw(0.0f)
	, m_Pitch(0.0f)
	, m_SensitivityX(1.0f)
	, m_SensitivityY(0.85f)
{
}

Camera::Camera(const CameraSettings& settings)
	: m_Position(settings.Position)
	, m_Front(settings.Front)
	, m_Right(settings.Right)
	, m_Up(settings.Up)
	, m_FOV(glm::radians(settings.FOV))
	, m_AspectRatio(settings.AspectRatio)
	, m_NearPlane(settings.NearPlane)
	, m_FarPlane(settings.FarPlane)
	, m_Speed(settings.Speed)
	, m_Yaw(settings.Yaw)
	, m_Pitch(settings.Pitch)
	, m_SensitivityX(settings.SensitivityX)
	, m_SensitivityY(settings.sensitivityY)
{
}

mat4 Camera::GetProjectionMatrix() const
{
	return glm::perspective(m_FOV, m_AspectRatio, m_NearPlane, m_FarPlane);
}

mat4 Camera::GetViewMatrix() const
{
	return glm::lookAt(
		m_Position,
		m_Position + m_Front,
		m_Up
	);
}


const vec3 X{ 1.0, 0.0, 0.0 };
const vec3 Y{ 0.0, 1.0, 0.0 };
const vec3 Z{ 0.0, 0.0, 1.0 };

void Camera::Update(EventHandler& events, f64 dt)
{
	vec3 desiredVelocity{ 0.0 };
	vec3 forward = glm::normalize(glm::vec3(m_Front.x, 0.0, m_Front.z));
	if (events.KeyPressed(KEY_Z)) desiredVelocity += forward;
	if (events.KeyPressed(KEY_Q)) desiredVelocity -= m_Right;
	if (events.KeyPressed(KEY_S)) desiredVelocity -= forward;
	if (events.KeyPressed(KEY_D)) desiredVelocity += m_Right;
	if (events.KeyPressed(KEY_SPACE)) desiredVelocity += Y;
	if (events.KeyPressed(KEY_CTRL) || events.KeyPressed(KEY_SHIFT)) desiredVelocity -= Y;
	
	if (desiredVelocity != vec3{ 0.0 })
	{
		desiredVelocity = glm::normalize(desiredVelocity) * m_Speed * (f32)dt;
		m_Position += desiredVelocity;
	}

	auto mouseRel = events.GetMouseRelative();
	if ((mouseRel.first != 0 || mouseRel.second != 0) && events.KeyPressed(MOUSE_RIGHT))
	{
		vec2 delta = glm::normalize(vec2(mouseRel.first, mouseRel.second));

		m_Yaw -= delta.x * m_SensitivityX;
		m_Pitch += delta.y * m_SensitivityY;

		m_Pitch = std::clamp(m_Pitch, -89.0f, 89.0f);
		m_Yaw = (f32)std::fmod(m_Yaw, 360);
		if (m_Yaw < 0) m_Yaw += 360;

		f32 yawRad = glm::radians(m_Yaw);
		f32 pitchRad = glm::radians(m_Pitch);

		vec3 desiredFront{};
		desiredFront.x = cos(yawRad) * cos(pitchRad);
		desiredFront.y = sin(pitchRad);
		desiredFront.z = sin(yawRad) * cos(pitchRad);

		m_Front = glm::normalize(desiredFront);
		m_Right = glm::normalize(glm::cross(m_Front, Y));
		m_Up = glm::normalize(glm::cross(m_Right, m_Front));
	}
}
