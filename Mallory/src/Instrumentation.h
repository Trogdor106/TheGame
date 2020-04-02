#pragma once
#include <string>
#include <fstream>
#include <spdlog/fmt/fmt.h>
#include <chrono>
#include "JsonProfiling.h"

typedef std::chrono::high_resolution_clock TimeClock;
typedef std::chrono::microseconds TimeResolution;
typedef std::chrono::time_point<TimeClock, TimeResolution> Timepoint;
inline Timepoint time_now() { return std::chrono::time_point_cast<TimeResolution>(TimeClock::now()); }

class ProfileTimer
{
public:
	ProfileTimer(const std::string& name, bool start = true);
	~ProfileTimer();

	void Restart();
	void Stop();

private:
	Timepoint   myStartTime;
	bool        isStopped;
	std::string myName;
};
