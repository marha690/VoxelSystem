// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldSlice.h"
#include "InfinitiveWorldGenerator.generated.h"

UCLASS()
class VOXELSYSTEM_API AInfinitiveWorldGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInfinitiveWorldGenerator();

	UPROPERTY(EditAnywhere)
		int renderDistance = 4;

	UPROPERTY(EditAnywhere)
		AActor* player;

	UPROPERTY(EditAnywhere)
		UMaterial* material;

	UPROPERTY(EditAnywhere)
		UTexture2D* custom_palette;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


private:
	UWorld* WRLD;
	int ActiveRenderDistance = 1;
	TMap<FVector2D, class AWorldSlice*>WorldSlices;
	TArray<FColor> colorAtlas;
	bool hasColorAtlas = true;

	int chunkSize = 32;
	int chunkDimension = 35;
	int chunksInHeight = 6;

	// Player.
	FVector2D PlayerAtSlice;
	FVector2D OldPlayerAtSlice;

	bool HasPlayerCrossedChunks();
	void DeleteUnnecessaryWorldSlices();
	void GenerateNewWorldSlices(int range);
	bool LoadSlice(FVector2D index);
	SliceState globalSliceState(int range);
	void readTexture();
	void setSliceNeighbours();
	void continueGenerationProcess(SliceState gState, int range);
	void renderMeshes(int range);
	bool generateArea(int range);
};
