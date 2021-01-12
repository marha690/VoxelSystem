// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chunk.h"

/**
 * 
 */
class VOXELSYSTEM_API denseChunk : public Chunk
{
public:
	denseChunk(int size, int vSize);
	denseChunk(int size, int vSize, VoxelData initalData);

	void setVoxel(VoxelData d, FVector xyz) override;
	void setVoxel(VoxelData d, int x, int y, int z) override;
	VoxelData getVoxel(FVector xyz) override;
	VoxelData getVoxel(int x, int y, int z) override;
	void setNeighbours(NeighbourChunk direction, denseChunk* chunk);
	bool hasNeighbour(NeighbourChunk direction);
	void updateMeshData() override;
	bool isSolid(int x, int y, int z) override;

	~denseChunk();

private:
	VoxelData* voxels;
	denseChunk* neighbour[NeighbourChunk::NUM_NEIGHBOURS] = {};

	void setAllVoxels(VoxelData data);
	VoxelData* findVoxel(int x, int y, int z);
	int convertVoxelToLocal(int i);

};


