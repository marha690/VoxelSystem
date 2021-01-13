// Fill out your copyright notice in the Description page of Project Settings.


#include "denseChunk.h"

denseChunk::denseChunk(int size, int vSize, int h, AWorldSlice* o)
	:Chunk(size, vSize), owner(o), heightIndex(h)
{
	voxels = new VoxelData[nVoxels];
	setAllVoxels(VoxelData{ VoxelType::AIR, 0 });
}

denseChunk::denseChunk(int size, int vSize, int h, VoxelData initalData, AWorldSlice* o)
	: Chunk(size, vSize), owner(o), heightIndex(h)
{
	voxels = new VoxelData[nVoxels];
	setAllVoxels(VoxelData{ VoxelType::AIR, 0 });
}

void denseChunk::setVoxel(VoxelData d, FVector xyz)
{
	setVoxel(d, xyz.X, xyz.Y, xyz.Z);
}

void denseChunk::setVoxel(VoxelData d, int x, int y, int z)
{
	meshCorrect = false;
	int index = arrayIndex(x, y, z);
	voxels[index] = d;
}

const VoxelData* denseChunk::getVoxel(FVector xyz)
{
	return getVoxel(xyz.X, xyz.Y, xyz.Z);
}

const VoxelData* denseChunk::getVoxel(int x, int y, int z)
{
	int index = arrayIndex(x, y, z);
	return &voxels[index];
}

void denseChunk::updateMeshData()
{
	if (!meshCorrect) {
		clearData();
		for (int x = 0; x < chunkSize; x++)
			for (int y = 0; y < chunkSize; y++)
				for (int z = 0; z < chunkSize; z++) {
					if (!isSolid(x, y, z)) continue;

					int index = arrayIndex(x, y, z);
					auto c = defaultPalette[voxels[index].colorID];
					FColor color(c);

					FVector voxelPos = FVector(x, y, z);
					if (!isSolid(voxelPos.X - 1, voxelPos.Y, voxelPos.Z))
						createQuad(BACK, voxelPos, color);
					if (!isSolid(voxelPos.X + 1, voxelPos.Y, voxelPos.Z))
						createQuad(FRONT, voxelPos, color);
					if (!isSolid(voxelPos.X, voxelPos.Y - 1, voxelPos.Z))
						createQuad(LEFT, voxelPos, color);
					if (!isSolid(voxelPos.X, voxelPos.Y + 1, voxelPos.Z))
						createQuad(RIGHT, voxelPos, color);
					if (!isSolid(voxelPos.X, voxelPos.Y, voxelPos.Z + 1))
						createQuad(TOP, voxelPos, color);
					if (!isSolid(voxelPos.X, voxelPos.Y, voxelPos.Z - 1))
						createQuad(BOTTOM, voxelPos, color);
				}
		meshCorrect = true;
	}
}

bool denseChunk::isSolid(int x, int y, int z)
{
	auto voxel = owner->getVoxel(x, y, z + heightIndex * chunkSize);
	if (voxel) {
		if (voxel->type == VoxelType::AIR ||
			voxel->type == VoxelType::IRREGULAR)
			return false;
	}

	return true;
}

denseChunk::~denseChunk()
{
	delete[] voxels;
}

void denseChunk::setAllVoxels(VoxelData data)
{
	meshCorrect = false;

	for (size_t i = 0; i < nVoxels; i++) {
		voxels[i] = data;
	}
}

int denseChunk::convertVoxelToLocal(int i)
{
	if (i <= -1) {
		i = chunkSize + i;
	}
	else if (i >= chunkSize) {
		i = i - chunkSize;
	}
	return i;
}

