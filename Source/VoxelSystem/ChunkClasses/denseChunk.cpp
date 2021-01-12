// Fill out your copyright notice in the Description page of Project Settings.


#include "denseChunk.h"

denseChunk::denseChunk(int size, int vSize)
	: Chunk(size, vSize) 
{
	voxels = new VoxelData[nVoxels];
	setAllVoxels(VoxelData{ VoxelType::AIR, 0 });
}

denseChunk::denseChunk(int size, int vSize, VoxelData initalData)
	: Chunk(size, vSize) 
{
	voxels = new VoxelData[nVoxels];
	setAllVoxels(initalData);
}

void denseChunk::setVoxel(VoxelData d, FVector xyz)
{
	setVoxel(d, xyz.X, xyz.Y, xyz.Z);
}

void denseChunk::setVoxel(VoxelData d, int x, int y, int z)
{
	meshCorrect = false;

	//int index = arrayIndex(x, y, z);
	//voxels[index] = d;

	auto v = findVoxel(x, y, z);
	if (v) {
		v->colorID = d.colorID;
		v->type= d.type;
	}
}

VoxelData denseChunk::getVoxel(FVector xyz)
{
	return getVoxel(xyz.X, xyz.Y, xyz.Z);
}

VoxelData denseChunk::getVoxel(int x, int y, int z)
{
	int index = arrayIndex(x, y, z);
	return voxels[index];
}

void denseChunk::setNeighbours(NeighbourChunk direction, denseChunk* chunk)
{
	neighbour[(int)direction] = chunk;
}

bool denseChunk::hasNeighbour(NeighbourChunk direction)
{
	return (neighbour[(int)direction] == nullptr) ? true: false;
}

void denseChunk::updateMeshData()
{
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

bool denseChunk::isSolid(int x, int y, int z)
{
	auto voxel = findVoxel(x, y, z);
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
		voxels[i].type = data.type;
		voxels[i].colorID = data.colorID;
	}
}

VoxelData* denseChunk::findVoxel(int x, int y, int z)
{

	if (x >= chunkSize || y >= chunkSize || z >= chunkSize || x < 0 || y < 0 || z < 0) { // Inside other chunk.

		FVector chunkOffset = FVector(0, 0, 0);
		if (x < 0) {
			chunkOffset -= FVector(1, 0, 0);
			x = convertVoxelToLocal(x);
		}
		else if (x > chunkSize - 1) {
			chunkOffset += FVector(1, 0, 0);
			x = convertVoxelToLocal(x);
		}

		if (y < 0) {
			chunkOffset -= FVector(0, 1, 0);
			y = convertVoxelToLocal(y);
		}
		else if (y > chunkSize - 1) {
			chunkOffset += FVector(0, 1, 0);
			y = convertVoxelToLocal(y);
		}

		if (z < 0) {
			chunkOffset -= FVector(0, 0, 1);
			z = convertVoxelToLocal(z);
		}
		else if (z > chunkSize - 1) {
			chunkOffset += FVector(0, 0, 1);
			z = convertVoxelToLocal(z);
		}

		NeighbourChunk neighbourArrayIndex;
		for (size_t i = 0; i < NeighbourChunk::NUM_NEIGHBOURS; i++)
		{
			if (chunkOffset == FVector(1, 0, 0))
				neighbourArrayIndex = NeighbourChunk::X_PLUS;
			if (chunkOffset == FVector(-1, 0, 0))
				neighbourArrayIndex = NeighbourChunk::X_MINUS;
			if (chunkOffset == FVector(0, 1, 0))
				neighbourArrayIndex = NeighbourChunk::Y_PLUS;
			if (chunkOffset == FVector(0, -1, 0))
				neighbourArrayIndex = NeighbourChunk::Y_MINUS;
			if (chunkOffset == FVector(0, 0, 1))
				neighbourArrayIndex = NeighbourChunk::Z_PLUS;
			if (chunkOffset == FVector(0, 0, -1))
				neighbourArrayIndex = NeighbourChunk::Z_MINUS;

			denseChunk *c = neighbour[(int)neighbourArrayIndex];
			if (c != nullptr) {
				int index = arrayIndex(x, y, z);
				return &c->voxels[index];
			}
			else
				return nullptr;
		}
	}

	// Inside chunk
	int index = arrayIndex(x, y, z);
	return &voxels[index];
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

