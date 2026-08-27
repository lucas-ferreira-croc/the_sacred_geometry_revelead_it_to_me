#include "timer.h"

void Timer::start()
{
	if(m_Running)
	{
		return;
	}
	m_Running = true;
	m_StartTime = std::chrono::steady_clock::now();
}

float Timer::stop()
{
	if(!m_Running)
	{
		return 0;
	}

	m_Running = false;
	auto stopTime = std::chrono::steady_clock::now();
	float timerMiliseconds = std::chrono::duration_cast<std::chrono::microseconds>(stopTime - m_StartTime).count() / 1000.0f;
	return timerMiliseconds;
}

