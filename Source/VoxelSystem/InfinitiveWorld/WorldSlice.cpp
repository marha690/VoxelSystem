// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldSlice.h"
#include "../Generation/Generator_1.h"
#include "../Generation/VoxReader.h"

// Sets default values
AWorldSlice::AWorldSlice()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CustomMesh = CreateDefaultSubobject<UProceduralMeshComponent>("CustomMesh");
	SetRootComponent(CustomMesh);
	CustomMesh->bUseAsyncCooking = true;
}

void AWorldSlice::initialize(int numChunks, int cSize, int chunkDimension, FVector2D cIndex)
{
	chunkSize = cSize;
	chunkHeight = numChunks;
	chunkIndex = cIndex;
	VoxelData air = VoxelData{ VoxelType::AIR, 0 };

	for (size_t i = 0; i < chunkHeight; i++) {
		denseChunk* c = new denseChunk(chunkSize, chunkDimension, i, air, this);
		chunks.Add(c);
	}

	state = SliceState::EMPTY;
}

void AWorldSlice::BeginPlay()
{
	Super::BeginPlay();

}

void AWorldSlice::setVoxel(VoxelData d, FVector xyz)
{
	setVoxel(d, xyz.X, xyz.Y, xyz.Z);
}

void AWorldSlice::setVoxel(VoxelData d, int x, int y, int z)
{
	auto slice = findSlice(x, y);
	x = convertVoxelToLocal(x);
	y = convertVoxelToLocal(y);

	if (slice) {
		int i = (double)z / (double)chunkSize;
		z = convertVoxelToLocal(z);
		if (slice->chunks[i])
			slice->chunks[i]->setVoxel(d, x, y, z);
	}
}

const VoxelData* AWorldSlice::getVoxel(int x, int y, int z)
{
	auto slice = findSlice(x, y);
	x = convertVoxelToLocal(x);
	y = convertVoxelToLocal(y);

	if (slice) {
		int i = (double)z / (double)chunkSize;
		z = convertVoxelToLocal(z);
		if(chunks[i])
			return slice->chunks[i]->getVoxel(x, y, z);
	}

	return nullptr;
}

void AWorldSlice::continueGenerationProcess(SliceState s)
{
	SliceState taskState = state;
	if (taskState == s) {
		state = SliceState::PROCESSING;
		(new FAutoDeleteAsyncTask<ChunkTask>(this, taskState))->StartBackgroundTask(); //Thread.
	}
}

void AWorldSlice::drawMesh()
{
	for (size_t i = 0; i < chunkHeight; i++)
	{
		chunks[i]->updateMeshData();

		TArray<FVector>* v = chunks[i]->getVertices();
		for (FVector& _v : *v)
			_v.Z += i * chunks[i]->getChunkWorldSize();

		CustomMesh->CreateMeshSection_LinearColor(i,
			*v,
			*chunks[i]->getTriangles(),
			*chunks[i]->getNormals(),
			*chunks[i]->getUVs(),
			*chunks[i]->getVertexColors(),
			TArray<FProcMeshTangent>(), true);
	}
	state = DONE;
}

void AWorldSlice::setNeighbour(NeighbourSlice direction, AWorldSlice* s)
{
	if (!neighbour[(int)direction])
		neighbour[(int)direction] = s;
}

AWorldSlice* AWorldSlice::findSlice(int x, int y)
{
	// x
	if (x < 0) {
		if (neighbour[X_M])
			return neighbour[X_M]->findSlice(chunkSize + x, y);
		else
			return nullptr;
	}
	else if (x > chunkSize - 1) {
		if (neighbour[X_P])
			return neighbour[X_P]->findSlice(x - chunkSize, y);
		else
			return nullptr;
	}

	// y
	if (y < 0) {
		if (neighbour[Y_M])
			return neighbour[Y_M]->findSlice(x, chunkSize + y);
		else
			return nullptr;
	}
	else if (y > chunkSize - 1) {
		if (neighbour[Y_P])
			return neighbour[Y_P]->findSlice(x, y - chunkSize);
		else
			return nullptr;
	}

	return this;
}

int AWorldSlice::convertVoxelToLocal(int i)
{
	if (i <= -1) {
		int n = abs((double)i / (double)chunkSize) + 1;
		i = chunkSize * n + i;
	}
	else if (i >= chunkSize) {
		int n = abs((double)i / (double)chunkSize);
		i = i - chunkSize * n;
	}
	return i;
}

/******************
***	ChunkTask	***
******************/

ChunkTask::ChunkTask(AWorldSlice* sl, SliceState st)
	: slice(sl), processState(st) {}

void ChunkTask::DoWork()
{
	switch (processState)
	{
	case PROCESSING:
		break;
	case EMPTY:
		MakeTerrain();
		break;
	case TERRAIN:
		MakeObjects();
		break;
	case STRUCTURES:
		MakeMesh();
		break;
	case MESH:
		break;
	case DONE:
		break;
	default:
		break;
	}

	return;
}

void ChunkTask::MakeTerrain()
{
	VoxelData stone = VoxelData{ VoxelType::REGULAR, 4 };
	VoxelData bottom = VoxelData{ VoxelType::UNBREAKABLE, 4 };

	Generator_1 g{};
	auto chunkIndex = slice->chunkIndex * slice->chunkSize;

	for (size_t x = 0; x < slice->chunkSize; x++)
		for (size_t y = 0; y < slice->chunkSize; y++) {

			int h = g.heightMap(chunkIndex.X + x, chunkIndex.Y + y);
			for (size_t z = 0; z < slice->chunkHeight * slice->chunkSize; z++) {
				if (h > z)
					slice->setVoxel(stone, x, y, z);
			}
		}

	slice->state = TERRAIN;
}

void ChunkTask::MakeObjects()
{
	VoxReader reader("test64.vox");
	Generator_1 g{};

	auto globalIndex = slice->chunkIndex * slice->chunkSize;

	for (size_t x = 0; x < slice->chunkSize; x++)
		for (size_t y = 0; y < slice->chunkSize; y++) {
			if (g.makeTree(globalIndex.X + x, globalIndex.Y + y)) {

				int zStart = g.heightMap(globalIndex.X + x, globalIndex.Y + y);
				for (size_t i = 0; i < reader.voxels.size(); i++) {
					auto raw = reader.voxels[i];
					VoxelData data{ VoxelType::REGULAR, raw.second };

					slice->setVoxel(data, raw.first.X + x, raw.first.Y + y, zStart + raw.first.Z);
				}
			}
		}

	slice->state = MESH;
}

void ChunkTask::MakeMesh()
{
	for (size_t i = 0; i < slice->chunkHeight; i++)
	{
		slice->chunks[i]->updateMeshData();
	}
	slice->state = MESH;
}
