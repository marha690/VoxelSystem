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

void AWorldSlice::initialize(int numChunks, int chunkSize, int chunkDimension, FVector2D cIndex)
{
	chunkHeight = numChunks;
	chunkIndex = cIndex;
	VoxelData air = VoxelData{ VoxelType::AIR, 0 };

	for (size_t i = 0; i < chunkHeight; i++) {
		denseChunk* c = new denseChunk(chunkSize, chunkDimension, air);
		chunks.Add(c);
	}
	
	//Set internal neigours.
	for (size_t i = 0; i < chunkHeight; i++) {
		if (i == 0) // bottom
			chunks[i]->setNeighbours(NeighbourChunk::Z_PLUS, chunks[i + 1]);
		else if ( i == chunkHeight - 1) // top
			chunks[i]->setNeighbours(NeighbourChunk::Z_MINUS, chunks[i - 1]);
		else {
			chunks[i]->setNeighbours(NeighbourChunk::Z_PLUS, chunks[i + 1]);
			chunks[i]->setNeighbours(NeighbourChunk::Z_MINUS, chunks[i - 1]);
		}
	}
	state = SliceState::EMPTY;
}

// Called when the game starts or when spawned
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
	int i = (double)z / (double)chunks[0]->chunkSize;
	int newZ = z - i * chunks[0]->chunkSize;
	chunks[i]->setVoxel(d, x, y, newZ);
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
		if (chunks[i]->isMeshOutdated()) {
			chunks[i]->updateMeshData();
		}

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

void AWorldSlice::setNeighbour(NeighbourSlice direction, AWorldSlice* slice)
{
	for (size_t i = 0; i < chunkHeight; i++)
	{
		if(chunks[i]->hasNeighbour((NeighbourChunk)direction))
			chunks[i]->setNeighbours((NeighbourChunk)direction, slice->chunks[i]);
	}
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
	auto chunkIndex = slice->chunkIndex;
	chunkIndex *= slice->chunks[0]->chunkSize;

	for (size_t x = 0; x < slice->chunks[0]->chunkSize; x++)
		for (size_t y = 0; y < slice->chunks[0]->chunkSize; y++) {

			//int h = g.heightMap(chunkIndex.X + x, chunkIndex.Y + y);
			//for (size_t z = 0; z < slice->chunkHeight * slice->chunks[0]->chunkSize; z++) {
			//	if (h > z)
			//		slice->setVoxel(stone, x, y, z);
			//}


					slice->setVoxel(stone, x, y, 0); //TODO: REMOVE
		}

	slice->state = TERRAIN;
}

void ChunkTask::MakeObjects()
{
	VoxReader reader("test64.vox");
	Generator_1 g{};


	auto globalIndex = slice->chunkIndex * slice->chunks[0]->chunkSize;

	for (size_t x = 0; x < slice->chunks[0]->chunkSize; x++)
		for (size_t y = 0; y < slice->chunks[0]->chunkSize; y++) {
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
		if (slice->chunks[i]->isMeshOutdated()) {
			slice->chunks[i]->updateMeshData();
		}
	}
	slice->state = MESH;
}
