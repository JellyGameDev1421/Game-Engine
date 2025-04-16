#pragma once

namespace DemoEngine
{
	struct SystemUsage
	{
		float cpuUsagePercent = 0.0f;
		size_t memoryUsageBytes = 0;
	};

	class SystemStats
	{
	public:
		static void Update();              // Call once per frame
		static const SystemUsage& Get();   // Returns the latest stats

	private:
		static SystemUsage s_Usage;
	};
}
