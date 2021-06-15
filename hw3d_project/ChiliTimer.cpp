#include "ChiliTimer.h"

using namespace std::chrono;

ChiliTimer::ChiliTimer()
{
	last = steady_clock::now();
}

float ChiliTimer::Mark()
{
	steady_clock::time_point old = last;
	last = steady_clock::now();
	duration<float> period = last - old;
	return period.count();
}

float ChiliTimer::Peek()
{
	duration<float> period = steady_clock::now() - last;
	return period.count();
}

