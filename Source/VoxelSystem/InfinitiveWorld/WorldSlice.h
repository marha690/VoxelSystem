// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "../ChunkClasses/denseChunk.h"

#include "WorldSlice.generated.h"

enum SliceState
{
	PROCESSING = 0,
	EMPTY = 1,
	TERRAIN = 2,
	STRUCTURES = 3,
	MESH = 4,
	REDRAW = 5,
	DONE
};

const static enum NeighbourSlice {
	X_P = 0,
	X_M = 1,
	Y_P = 2,
	Y_M = 3,

	NUM_SLICES
};

UCLASS()
class VOXELSYSTEM_API AWorldSlice : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorldSlice();
	void initialize(int numChunks, int chunkSize, int chunkDimension, FVector2D cIndex);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	void setVoxel(VoxelData d, FVector xyz);
	void setVoxel(VoxelData d, int x, int y, int z);
	void continueGenerationProcess(SliceState s);
	void drawMesh();
	SliceState getState() { return state; }
	void setNeighbour(NeighbourSlice direction, AWorldSlice* slice);

private:
	int chunkHeight;
	FVector2D chunkIndex;
	TArray<denseChunk*> chunks;
	UProceduralMeshComponent* CustomMesh;
	SliceState state = PROCESSING;

	bool neighboursSet = false;
	friend class ChunkTask;
};

/******************
***	ChunkTask	***
******************/
class ChunkTask : public FNonAbandonableTask
{
public:

	ChunkTask(AWorldSlice* sl, SliceState st);

	FORCEINLINE TStatId GetStatId() const {
		RETURN_QUICK_DECLARE_CYCLE_STAT(ChunkTask, STATGROUP_ThreadPoolAsyncTasks);
	}
	void DoWork();

private:
	void MakeTerrain();
	void MakeObjects();
	void MakeMesh();
	AWorldSlice* slice;
	SliceState processState;
};