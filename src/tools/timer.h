#ifndef TIMER_H
#define TIMER_H

#include <chrono>

class Timer
{
public:
	void start();
	float stop();
private:
	bool m_Running = false;
	std::chrono::time_point<std::chrono::steady_clock> m_StartTime;
};

#endif