// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

// VOXEL //
const static enum VoxelType {
	AIR = 0,
	REGULAR = 1,
	IRREGULAR = 2,
	UNBREAKABLE = 3,

	NUM_TYPES
};
struct VoxelData
{
	VoxelType type;
	uint8_t colorID;
};

const static enum NeighbourChunk {
	X_PLUS = 0,
	X_MINUS = 1,	
	Y_PLUS = 2,
	Y_MINUS = 3,	
	Z_PLUS = 4,
	Z_MINUS = 5,

	NUM_NEIGHBOURS
};
/**
 *	CHUNK CLASS
 */
class VOXELSYSTEM_API Chunk
{
public:

	int chunkSize; // number of voxels in one dimension of a chunk
	int nVoxels; // all voxels inside this chunk
	int voxelSize; //worlds length of a voxel.

	// function variables
	Chunk(int size, int vSize);
	virtual ~Chunk() {};
	virtual void setVoxel(VoxelData d, FVector xyz) { verify(false); }
	virtual void setVoxel(VoxelData d, int x, int y, int z) { verify(false); }
	virtual const VoxelData* getVoxel(FVector xyz) { return nullptr; }
	virtual const VoxelData* getVoxel(int x, int y, int z) { return nullptr; }
	virtual bool isSolid(int x, int y, int z) { return false; }
	virtual void updateMeshData() { verify(false); }
	int getChunkWorldSize() {return voxelSize * chunkSize; }
	void clearData();
	TArray<FVector>* getVertices() { return &vertices; }
	TArray<int32>* getTriangles() { return &triangles; }
	TArray<FLinearColor>* getVertexColors() { return &vertexColors; }
	TArray<FVector>* getNormals() { return &normals; }
	TArray<FVector2D>* getUVs() { return &uv0; }

protected:
	const static enum Cubeside { BOTTOM, TOP, LEFT, RIGHT, FRONT, BACK };
	bool meshCorrect = false;

	void createQuad(Cubeside side, FVector indexInChunk, FColor color);
	void addTriangle(int32 V1, int32 V2, int32 V3);
	int arrayIndex(FVector xyz);
	int arrayIndex(int x, int y, int z);

private:
	// UVs
	static const FVector2D uv00;
	static const FVector2D uv10;
	static const FVector2D uv01;
	static const FVector2D uv11;
	// Mesh variables
	TArray<FVector> vertices;
	TArray<int32> triangles;
	TArray<FLinearColor> vertexColors;
	TArray<FVector> normals;
	TArray<FVector2D> uv0;
};
