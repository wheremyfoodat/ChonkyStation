#pragma once
#pragma warning(disable : 4996)
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <fstream>

#define SECTOR_SIZE 0x930
#define CDXA_DATA_SIZE 0x800

struct CD
{
	int BytesRead = 0;
	uint8_t drqsts = 0;
	bool WholeSector = false;
	bool IsCDInserted = false;
	uint8_t SectorBuffer[SECTOR_SIZE];

	const char* Dir;
	FILE* iso;

	CD();
	void OpenFile(const char* directory);
	void read(uint32_t loc);
	uint8_t ReadDataByte();
};

