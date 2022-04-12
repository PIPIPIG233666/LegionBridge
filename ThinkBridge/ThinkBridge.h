#include <string>
using namespace std;
#include <comutil.h>


DWORD deax, debx, decx, dedx;

struct FCSTATE {
	char Fan1StateLevel,
		Fan2StateLevel,
		FanSpeedLo1,
		FanSpeedHi1,
		FanSpeedLo2,
		FanSpeedHi2;
	int Fan1Speed,
		Fan2Speed;
} State;

extern "C" __declspec(dllexport) int StartDevice();

extern "C" __declspec(dllexport) BSTR ReadCpuName();

extern "C" __declspec(dllexport) BSTR ReadGpuName();

extern "C" __declspec(dllexport) int ReadCpuTemperture();

extern "C" __declspec(dllexport) int ReadGpuTemperture();

extern "C" __declspec(dllexport) int ReadFan1Speed();

extern "C" __declspec(dllexport) int ReadFan2Speed();

extern "C" __declspec(dllexport) int SetFan1State(int fanState);

extern "C" __declspec(dllexport) int SetFan2State(int fanState);
