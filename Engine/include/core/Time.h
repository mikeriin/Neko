#ifndef TIME_H
#define TIME_H


#include <chrono>


#include "Neko.h"


class Time
{
public:
	Time();
	~Time() = default;

	void SetMaxFPS(i32 maxFPS);
	void Update();
	
	static f64 DeltaTime;
	static f64 FixedDeltaTime;

private:
	std::chrono::steady_clock::time_point m_LastFrameTime;

	bool m_IsFPSMaxed;
	f64 m_DesiredFrameTime;
};



#endif // !TIME_H
