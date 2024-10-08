#include "core/Time.h"


#include <iostream>
#include <thread>


f64 Time::DeltaTime{ 0.0 };
f64 Time::FixedDeltaTime{ 0.0 };


Time::Time()
	: m_LastFrameTime(std::chrono::high_resolution_clock::now())
	, m_IsFPSMaxed(false)
	, m_DesiredFrameTime(0.0)
{
}

void Time::SetMaxFPS(i32 maxFPS)
{
	if (maxFPS == 0) return;

	m_IsFPSMaxed = true;
	m_DesiredFrameTime = 1.0 / maxFPS;
	FixedDeltaTime = m_DesiredFrameTime;
}

void Time::Update()
{
	auto currentFrameTime = std::chrono::high_resolution_clock::now();
	DeltaTime = std::chrono::duration<f64>(currentFrameTime - m_LastFrameTime).count();

	if (m_IsFPSMaxed)
	{
		f64 sleepTime = m_DesiredFrameTime - DeltaTime;
		if (sleepTime > 0)
		{
			std::this_thread::sleep_for(std::chrono::duration<f64>(sleepTime));
			DeltaTime = std::chrono::duration<f64>(currentFrameTime - m_LastFrameTime).count();
		}
	}

	m_LastFrameTime = currentFrameTime;
}
