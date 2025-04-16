#include "DemoEngine_PCH.h"
#include "SystemStats.h"
#include <Windows.h>
#include <psapi.h>

namespace DemoEngine
{
	SystemUsage SystemStats::s_Usage;

	void SystemStats::Update()
	{
		// CPU usage (simplified, per process - not highly accurate)
		static ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
		static int numProcessors;
		static HANDLE self;

		if (numProcessors == 0)
		{
			SYSTEM_INFO sysInfo;
			GetSystemInfo(&sysInfo);
			numProcessors = sysInfo.dwNumberOfProcessors;

			self = GetCurrentProcess();

			FILETIME ftime, fsys, fuser;
			GetSystemTimeAsFileTime(&ftime);
			GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
			memcpy(&lastCPU, &ftime, sizeof(FILETIME));
			memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
			memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));
		}

		FILETIME ftime, fsys, fuser;
		ULARGE_INTEGER now, sys, user;
		GetSystemTimeAsFileTime(&ftime);
		memcpy(&now, &ftime, sizeof(FILETIME));

		GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
		memcpy(&sys, &fsys, sizeof(FILETIME));
		memcpy(&user, &fuser, sizeof(FILETIME));

		double percent = (double)((sys.QuadPart - lastSysCPU.QuadPart) +
			(user.QuadPart - lastUserCPU.QuadPart));
		percent /= (now.QuadPart - lastCPU.QuadPart);
		percent /= numProcessors;
		percent *= 100.0;

		lastCPU = now;
		lastUserCPU = user;
		lastSysCPU = sys;

		s_Usage.cpuUsagePercent = (float)percent;

		// Memory usage
		PROCESS_MEMORY_COUNTERS_EX pmc;
		if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
		{
			s_Usage.memoryUsageBytes = pmc.PrivateUsage;
		}
	}

	const SystemUsage& SystemStats::Get()
	{
		return s_Usage;
	}
}
