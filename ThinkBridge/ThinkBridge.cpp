#include "_prec.h"
#include "lib/nvapi/nvapi.h"
#include "ThinkBridge.h"
#include <atlconv.h>
#include "PoC.h"

#define CPUTEMP "0xFE0B0538"
#define GPUTEMP "0xFE0B0539"

#define FANPWRMODE "0xFE0B0530"
#define ECPWRMODE "0xFE0B0420"

#define CPUFANSPEED "0xFE0B0600"
#define GPUFANSPEED "0xFE0B0601"

const char* CPUCurrentFanCurve[]{
	"0xFE0B0540",
	"0xFE0B0541",
	"0xFE0B0542",
	"0xFE0B0543",
	"0xFE0B0544",
	"0xFE0B0545",
	"0xFE0B0546",
	"0xFE0B0547",
	"0xFE0B0548",
	"0xFE0B0549",
};

const char* GPUCurrentFanCurve[]{
	"0xFE0B0550",
	"0xFE0B0551",
	"0xFE0B0552",
	"0xFE0B0553",
	"0xFE0B0554",
	"0xFE0B0555",
	"0xFE0B0556",
	"0xFE0B0557",
	"0xFE0B0558",
	"0xFE0B0559",
};

RwDrv::RwDrv()
{
	h = CreateFileA("\\\\.\\RwDrv", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (h == INVALID_HANDLE_VALUE)
	{
		printf("Driver Not Loaded!\n");
		exit(0);
	}
}

RwDrv::~RwDrv()
{
	CloseHandle(h);
}

void RwDrv::PhysicalRead(uint64_t Address, uint64_t* Address2, DWORD Size)
{
	PhysRW_t A;

	A.PhysicalAddress = Address;
	A.Address = (uint64_t)Address2;
	A.Unknown = 0;
	A.Size = Size;

	DeviceIoControl(h, 0x222808, &A, sizeof(A), &A, sizeof(A), 0, 0);
}

void RwDrv::PhysicalWrite(uint64_t Address, uint64_t* Address2, DWORD Size)
{
	PhysRW_t A;

	A.PhysicalAddress = Address;
	A.Address = (uint64_t)Address2;
	A.Unknown = 0;
	A.Size = Size;

	DeviceIoControl(h, 0x22280C, &A, sizeof(A), 0, 0, 0, 0);
}

//-------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------
BSTR convertCharToBSTR(char* input) {

	USES_CONVERSION;
	const OLECHAR* pOleChar = A2CW(input);
	BSTR str = SysAllocString(pOleChar);
	SysFreeString(str);

	return str;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
int StartDevice()
{
	NvAPI_Status nvapi_ok = NVAPI_ERROR;

	for (int i = 0; i < 180; i++)
	{
		nvapi_ok = NvAPI_Initialize();
		if (nvapi_ok == NVAPI_OK) break;

		::Sleep(1000);
	}

	return 0;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
BSTR ReadCpuName() {

	char _cpuName[48] = { 0 };

	const DWORD id = 0x80000002;

	memset(_cpuName, 0, sizeof(_cpuName));

	for (DWORD t = 0; t < 3; t++) {

		const DWORD veax = id + t;

		__asm
		{
			mov eax, veax
			cpuid
			mov deax, eax
			mov debx, ebx
			mov decx, ecx
			mov dedx, edx
		}

		memcpy(_cpuName + 16 * t + 0, &deax, 4);
		memcpy(_cpuName + 16 * t + 4, &debx, 4);
		memcpy(_cpuName + 16 * t + 8, &decx, 4);
		memcpy(_cpuName + 16 * t + 12, &dedx, 4);
	}


	return convertCharToBSTR(_cpuName);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
BSTR ReadGpuName()
{
	NvAPI_Status nvapi_ok = NVAPI_ERROR;

	// Get GPU Name
	NvU32 count;
	NvPhysicalGpuHandle handle;

	NvAPI_ShortString _gpuName;

	std::string a = "";

	nvapi_ok = NvAPI_EnumPhysicalGPUs(&handle, &count);
	if (nvapi_ok != NVAPI_OK) return NULL;

	nvapi_ok = NvAPI_GPU_GetFullName(handle, _gpuName);
	if (nvapi_ok != NVAPI_OK) return NULL;

	return convertCharToBSTR(_gpuName);

}

int PhysicalRead(const char *node) {
	RwDrv* Drv = new RwDrv();
	int ret = 0;
	for (int i = 2; i < 3; i++)
	{
		uint64_t Address;
		sscanf_s(node, "%llx", &Address);
		uint8_t result;
		Drv->PhysicalRead(Address, (uint64_t*)&result, 1);
		ret = static_cast<unsigned>(result);
	}
	return ret;
}

int PhysicalWrite(const char* node, const char *data) {
	RwDrv* Drv = new RwDrv();
	int ret = 0;
	for (int i = 2; i < 4; i += 2)
	{
		uint64_t Address;
		uint32_t Data;
		sscanf_s(node, "%llx", &Address);
		sscanf_s(data, "%x", &Data);
		Drv->PhysicalWrite(Address, (uint64_t*)&Data, 1);
	}
	return ret;
}

int SetFan1State(int fanstate)
{
	int ret = 0;

	for (int i = 0; i < 10; i++) {

		PhysicalWrite(CPUCurrentFanCurve[i], "0A");

		::Sleep(100);
	}

	return 0;
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
int SetFan2State(int fanstate)
{
	int ret = 0;

	for (int i = 0; i < 10; i++) {

		PhysicalWrite(GPUCurrentFanCurve[i], "0A");

		::Sleep(100);
	}

	return 0;
}

//-------------------------------------------------------------------------
//  
//-------------------------------------------------------------------------

int ReadCpuTemperture(int* cpuTemperture)
{
	*cpuTemperture = PhysicalRead(CPUTEMP);
	return 0;
}

int ReadGpuTemperture(int* gpuTemperture)
{
	*gpuTemperture = PhysicalRead(GPUTEMP);
	return 0;
}

int ReadPerfMode() {
	int ret = 0;

	return ret;
}

int ReadFanState(FCSTATE* state)
{
	state->Fan1Speed = 44 * (int)PhysicalRead(CPUFANSPEED);
	state->Fan2Speed = 44 * (int)PhysicalRead(GPUFANSPEED);

	return 0;
}

int main(void)
{
	return 0;
}


// interop warpper function
int ReadCpuTemperture()
{
	int cpuTemperture;
	ReadCpuTemperture(&cpuTemperture);
	return cpuTemperture;
}

int ReadGpuTemperture()
{
	int gpuTemperture;
	ReadGpuTemperture(&gpuTemperture);
	return gpuTemperture;
}

int ReadFan1Speed() {
	FCSTATE state;
	ReadFanState(&state);
	return state.Fan1Speed;
}


int ReadFan2Speed() {
	FCSTATE state;
	ReadFanState(&state);
	return state.Fan2Speed;
}

