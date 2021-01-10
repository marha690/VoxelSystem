// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldSlice.h"

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
	state = SliceState::Empty;
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
		state = SliceState::Processing;
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
	state = Done;
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
	case Processing:
		break;
	case Empty:
		MakeTerrain();
		break;
	case Terrain:
		slice->state = Structures;
		break;
	case Structures:
		MakeMesh();
		break;
	case Mesh:
		break;
	case Done:
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

	for (size_t i = 0; i < slice->chunks[0]->chunkSize; i++)
		for (size_t j = 0; j < slice->chunks[0]->chunkSize; j++)
		{
			slice->setVoxel(bottom, i, j, 0);
			slice->setVoxel(stone, i, j, 1);

			slice->setVoxel(stone, i, j, 32);
			slice->setVoxel(stone, i, j, 31);
		}
	slice->state = Terrain;
}

void ChunkTask::MakeMesh()
{
	for (size_t i = 0; i < slice->chunkHeight; i++)
	{
		if (slice->chunks[i]->isMeshOutdated()) {
			slice->chunks[i]->updateMeshData();
		}
	}
	slice->state = Mesh;
}
