// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chunk.h"
#include "../InfinitiveWorld/WorldSlice.h"

class AWorldSlice;
/**
 * 
 */
class VOXELSYSTEM_API denseChunk : public Chunk
{
public:
	denseChunk(int size, int vSize, int h, AWorldSlice* o);
	denseChunk(int size, int vSize, int h, VoxelData initalData, AWorldSlice* o);

	void setVoxel(VoxelData d, FVector xyz) override;
	void setVoxel(VoxelData d, int x, int y, int z) override;
	const VoxelData* getVoxel(FVector xyz) override;
	const VoxelData* getVoxel(int x, int y, int z) override;

	void updateMeshData() override;
	bool isSolid(int x, int y, int z) override;

	~denseChunk();

private:
	VoxelData* voxels;
	AWorldSlice* owner;
	int heightIndex;

	void setAllVoxels(VoxelData data);
	int convertVoxelToLocal(int i);
};


