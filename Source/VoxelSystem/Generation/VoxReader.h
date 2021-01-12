// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <string>
#include <vector>
/**
 * 
 */
class VOXELSYSTEM_API VoxReader
{
public:
	VoxReader(FString FileName);
	~VoxReader();

	std::vector<std::pair<FVector, uint8_t>> voxels;
	FVector ModelSize;

private:
	FString FolderName = "Vox/";
	void ReadData(FString FilePath);

	// Functions
	int MV_ID(int a, int b, int c, int d) {
		return (a) | (b << 8) | (c << 16) | (d << 24);
	}

	int MV_INT(int a, int b, int c, int d) {
		return (a << 24) | (b << 18) | (c << 8) | (d);
	}

	const int ID_VOX = MV_ID('V', 'O', 'X', ' ');
	const int ID_MAIN = MV_ID('M', 'A', 'I', 'N');
	const int ID_SIZE = MV_ID('S', 'I', 'Z', 'E');
	const int ID_XYZI = MV_ID('X', 'Y', 'Z', 'I');
	const int ID_RGBA = MV_ID('R', 'G', 'B', 'A');
	const int ID_PACK = MV_ID('P', 'A', 'C', 'K');

	enum MV_ChunkType { PACK, SIZE, XYZI, RGBA };

	std::string ConvertToString(char* a, int size);
};
