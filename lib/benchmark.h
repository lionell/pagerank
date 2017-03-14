#include <chrono>

#include <glog/logging.h>

using namespace std::chrono;
using namespace std;

class Timer {
	high_resolution_clock::time_point wall_start;
	high_resolution_clock::time_point wall_finish;
	clock_t cpu_start;
	clock_t cpu_finish;
	double overall_wall_time;
	double overall_cpu_time;
	size_t ticks;

public:
	Timer() : ticks(0) {}

	void Start() {
		wall_start = high_resolution_clock::now();
		cpu_start = clock();
	}

	void Stop() {
		wall_finish = high_resolution_clock::now();
		cpu_finish = clock();
		overall_wall_time += GetWallDuration();
		overall_cpu_time += GetCpuDuration();
		ticks++;
	}

	double GetWallDuration() {
		return duration_cast<milliseconds>(wall_finish - wall_start).count()
			/ 1000.0;
	}

	double GetCpuDuration() {
		return (cpu_finish - cpu_start) / double(CLOCKS_PER_SEC);
	}

	void Report(string event) {
		LOG(INFO) << event + " took " + to_string(GetCpuDuration())
			+ "s cpu, " + to_string(GetWallDuration()) + "s wall.";
	}

	void StopAndReport(string event) {
		Stop();
		Report(event);
	}

	void ReportStats(string event) {
		double average_wall_time = overall_wall_time / ticks;
		double average_cpu_time = overall_cpu_time / ticks;
		LOG(INFO) << "Overall " + event + " took " + to_string(overall_cpu_time)
			+ "s cpu, " + to_string(overall_wall_time) + "s wall.";
		LOG(INFO) << "Average " + event + " took " + to_string(average_cpu_time)
			+ "s cpu, " + to_string(average_wall_time) + "s wall.";
	}
};
