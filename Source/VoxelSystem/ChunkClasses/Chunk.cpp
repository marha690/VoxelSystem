// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunk.h"

const FVector2D Chunk::uv00 = FVector2D(0.f, 0.f);
const FVector2D Chunk::uv10 = FVector2D(1.f, 0.f);
const FVector2D Chunk::uv01 = FVector2D(0.f, 1.f);
const FVector2D Chunk::uv11 = FVector2D(1.f, 1.f);

Chunk::Chunk(int size, int vSize)
{
	chunkSize = size;
	voxelSize = vSize;
	nVoxels = size * size * size;
}

void Chunk::clearData()
{
	vertices.Empty();
	triangles.Empty();
	vertexColors.Empty();
	normals.Empty();
	uv0.Empty();
}

int Chunk::arrayIndex(FVector xyz)
{
	return arrayIndex(xyz.X, xyz.Y, xyz.Z);
}

int Chunk::arrayIndex(int x, int y, int z)
{
	return x + y * chunkSize + z * chunkSize * chunkSize;
}

void Chunk::createQuad(Cubeside side, FVector indexInChunk, FColor color)
{
	int32 startIndex = vertices.Num();

	FVector p0 = FVector(0 + indexInChunk.X * voxelSize, 0 + indexInChunk.Y * voxelSize, 0 + indexInChunk.Z * voxelSize); //lower left - 0
	FVector p1 = FVector(0 + indexInChunk.X * voxelSize, 0 + indexInChunk.Y * voxelSize, voxelSize + indexInChunk.Z * voxelSize); //upper left - 1
	FVector p2 = FVector(0 + indexInChunk.X * voxelSize, voxelSize + indexInChunk.Y * voxelSize, 0 + indexInChunk.Z * voxelSize); //lower right - 2 
	FVector p3 = FVector(0 + indexInChunk.X * voxelSize, voxelSize + indexInChunk.Y * voxelSize, voxelSize + indexInChunk.Z * voxelSize); //upper right - 3
	FVector p4 = FVector(voxelSize + indexInChunk.X * voxelSize, -0 + indexInChunk.Y * voxelSize, 0 + indexInChunk.Z * voxelSize); //lower front left - 4
	FVector p5 = FVector(voxelSize + indexInChunk.X * voxelSize, 0 + indexInChunk.Y * voxelSize, voxelSize + indexInChunk.Z * voxelSize); //upper front left - 5
	FVector p6 = FVector(voxelSize + indexInChunk.X * voxelSize, voxelSize + indexInChunk.Y * voxelSize, voxelSize + indexInChunk.Z * voxelSize); //upper front right - 6
	FVector p7 = FVector(voxelSize + indexInChunk.X * voxelSize, voxelSize + indexInChunk.Y * voxelSize, 0 + indexInChunk.Z * voxelSize); //lower front right - 7

	switch (side)
	{
	case Cubeside::BOTTOM:
		//Bottom face
		uv0.Add(uv00);
		uv0.Add(uv10);
		uv0.Add(uv01);
		uv0.Add(uv11);
		vertices.Add(p2);
		vertices.Add(p0);
		vertices.Add(p7);
		vertices.Add(p4);
		normals.Add(FVector::DownVector);
		normals.Add(FVector::DownVector);
		normals.Add(FVector::DownVector);
		normals.Add(FVector::DownVector);
		addTriangle(startIndex, startIndex + 1, startIndex + 2);
		addTriangle(startIndex + 3, startIndex + 2, startIndex + 1);
		vertexColors.Add(color);
		vertexColors.Add(color);
		vertexColors.Add(color);
		vertexColors.Add(color);
		break;
	case Cubeside::TOP:
		//Top face
		uv0.Add(uv00);
		uv0.Add(uv10);
		uv0.Add(uv01);
		uv0.Add(uv11);
		vertices.Add(p1);
		vertices.Add(p3);
		vertices.Add(p5);
		vertices.Add(p6);
		normals.Add(FVector::UpVector);
		normals.Add(FVector::UpVector);
		normals.Add(FVector::UpVector);
		normals.Add(FVector::UpVector);
		addTriangle(startIndex, startIndex + 1, startIndex + 2);
		addTriangle(startIndex + 3, startIndex + 2, startIndex + 1);
		vertexColors.Add(color);
		vertexColors.Add(color);
		vertexColors.Add(color);
		vertexColors.Add(color);
		break;
	case Cubeside::LEFT:
		//Left face of cube
		uv0.Add(uv00);
		uv0.Add(uv10);
		uv0.Add(uv01);
		uv0.Add(uv11);
		vertices.Add(p0);
		vertices.Add(p1);
		vertices.Add(p4);
		vertices.Add(p5);
		normals.Add(FVector::LeftVector);
		normals.Add(FVector::LeftVector);
		normals.Add(FVector::LeftVector);
		normals.Add(FVector::LeftVector);
		addTriangle(startIndex, startIndex + 1, startIndex + 2);
		addTriangle(startIndex + 3, startIndex + 2, startIndex + 1);
		vertexColors.Add(color);
		vertexColors.Add(color);
		vertexColors.Add(color);
		vertexColors.Add(color);
		break;
	case Cubeside::RIGHT:
		//Right face of cube
		uv0.Add(uv00);
		uv0.Add(uv10);
		uv0.Add(uv01);
		uv0.Add(uv11);
		vertices.Add(p7);
		vertices.Add(p6);
		vertices.Add(p2);
		vertices.Add(p3);
		normals.Add(FVector::RightVector);
		normals.Add(FVector::RightVector);
		normals.Add(FVector::RightVector);
		normals.Add(FVector::RightVector);
		addTriangle(startIndex, startIndex + 1, startIndex + 2);
		addTriangle(startIndex + 3, startIndex + 2, startIndex + 1);
		vertexColors.Add(color);
		vertexColors.Add(color);
		vertexColors.Add(color);
		vertexColors.Add(color);
		break;
	case Cubeside::FRONT:
		//Right face of cube
		uv0.Add(uv00);
		uv0.Add(uv10);
		uv0.Add(uv01);
		uv0.Add(uv11);
		vertices.Add(p4);
		vertices.Add(p5);
		vertices.Add(p7);
		vertices.Add(p6);
		normals.Add(FVector::ForwardVector);
		normals.Add(FVector::ForwardVector);
		normals.Add(FVector::ForwardVector);
		normals.Add(FVector::ForwardVector);
		addTriangle(startIndex, startIndex + 1, startIndex + 2);
		addTriangle(startIndex + 3, startIndex + 2, startIndex + 1);
		vertexColors.Add(color);
		vertexColors.Add(color);
		vertexColors.Add(color);
		vertexColors.Add(color);
		break;
	case Cubeside::BACK:
		//Back face of cube
		uv0.Add(uv00);
		uv0.Add(uv10);
		uv0.Add(uv01);
		uv0.Add(uv11);
		vertices.Add(p3);
		vertices.Add(p1);
		vertices.Add(p2);
		vertices.Add(p0);
		normals.Add(FVector::BackwardVector);
		normals.Add(FVector::BackwardVector);
		normals.Add(FVector::BackwardVector);
		normals.Add(FVector::BackwardVector);
		addTriangle(startIndex, startIndex + 1, startIndex + 2);
		addTriangle(startIndex + 3, startIndex + 2, startIndex + 1);
		vertexColors.Add(color);
		vertexColors.Add(color);
		vertexColors.Add(color);
		vertexColors.Add(color);
		break;
	}
}

void Chunk::addTriangle(int32 V1, int32 V2, int32 V3)
{
	triangles.Add(V1);
	triangles.Add(V2);
	triangles.Add(V3);
}
