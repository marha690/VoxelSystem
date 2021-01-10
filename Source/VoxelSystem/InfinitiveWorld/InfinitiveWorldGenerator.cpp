// Fill out your copyright notice in the Description page of Project Settings.


#include "InfinitiveWorldGenerator.h"

// Sets default values
AInfinitiveWorldGenerator::AInfinitiveWorldGenerator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AInfinitiveWorldGenerator::BeginPlay()
{
	Super::BeginPlay();

	WRLD = GetWorld(); // Get current world.
	verify(WRLD != nullptr);

	int chunkWorldSize = chunkSize * chunkDimension;
	OldPlayerAtSlice.X = (int)floor(player->GetTransform().GetLocation().X / chunkWorldSize);
	OldPlayerAtSlice.Y = (int)floor(player->GetTransform().GetLocation().Y / chunkWorldSize);
}

// Called every frame
void AInfinitiveWorldGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// update player position
	int chunkWorldSize = chunkSize * chunkDimension;
	PlayerAtSlice.X = (int)floor(player->GetTransform().GetLocation().X / chunkWorldSize);
	PlayerAtSlice.Y = (int)floor(player->GetTransform().GetLocation().Y / chunkWorldSize);


	if (HasPlayerCrossedChunks()) {
		ActiveRenderDistance = 1; // Reset render distance.
		DeleteUnnecessaryWorldSlices();
		OldPlayerAtSlice = PlayerAtSlice;
	}

	// loop for generating the world.
	GenerateNewWorldSlices(ActiveRenderDistance + 1);
	setChunkNeighbours();

	//SliceState padding = globalSliceState(ActiveRenderDistance + 1);

	SliceState g = globalSliceState(ActiveRenderDistance);
	switch (g)
	{
	case Processing:
		break;
	case Empty:
		continueGenerationProcess(g, ActiveRenderDistance + 1);
		break;
	case Terrain:
		continueGenerationProcess(g, ActiveRenderDistance);
		break;
	case Structures:
		continueGenerationProcess(g, ActiveRenderDistance);
		break;
	case Mesh: {
		renderMeshes(ActiveRenderDistance);
		break;
	}
	case Done: {
		if (ActiveRenderDistance < renderDistance) {
			ActiveRenderDistance = ActiveRenderDistance + 1;
		}
		break;
	}
	default:
		break;
	}
}

bool AInfinitiveWorldGenerator::HasPlayerCrossedChunks()
{
	if (PlayerAtSlice.X != OldPlayerAtSlice.X || PlayerAtSlice.Y != OldPlayerAtSlice.Y)
		return true;
	else
		return false;
}

void AInfinitiveWorldGenerator::DeleteUnnecessaryWorldSlices()
{
	for (auto It = WorldSlices.CreateConstIterator(); It; ++It)
	{
		int x = It.Key().X;
		int y = It.Key().Y;

		if (abs(x - PlayerAtSlice.X) > renderDistance ||
			abs(y - PlayerAtSlice.Y) > renderDistance) {

			WorldSlices.Remove(It.Key());
			It.Value()->Destroy();
		}
	}
}

void AInfinitiveWorldGenerator::GenerateNewWorldSlices(int range)
{
	for (int x = PlayerAtSlice.X - range; x <= PlayerAtSlice.X + range; x++)
		for (int y = PlayerAtSlice.Y - range; y <= PlayerAtSlice.Y + range; y++) {
			LoadSlice({ (float)x, (float)y });
		}
}

bool AInfinitiveWorldGenerator::LoadSlice(FVector2D index)
{
	// Check if the chunk already is loaded.
	if (WorldSlices.Contains(index)) {
		return false;
	}

	// Create new chunk if it does not exist.
	FRotator rot = FRotator(0, 0, 0);
	FVector pos = FVector(index, 0) * (chunkSize * chunkDimension);
	auto v = (AWorldSlice*)GetWorld()->SpawnActor(AWorldSlice::StaticClass(), &pos, &rot);
	v->initialize(4, chunkSize, chunkDimension, index);
	v->SetFolderPath("/Chunks");
	WorldSlices.Add(index, v);
	return true;
}

SliceState AInfinitiveWorldGenerator::globalSliceState(int range)
{
	SliceState globalProgress = Done;

	for (int x = PlayerAtSlice.X - range; x <= PlayerAtSlice.X + range; x++)
		for (int y = PlayerAtSlice.Y - range; y <= PlayerAtSlice.Y + range; y++) {
			auto slice = WorldSlices.Find({ (float)x, (float)y });
			if (slice) {
				auto s = (*slice)->getState();
				int localProgress = (int)s;
				if (localProgress < (int)globalProgress)
					globalProgress = s;
			}
		}

	return globalProgress;
}

void AInfinitiveWorldGenerator::setChunkNeighbours()
{
	for (auto It = WorldSlices.CreateConstIterator(); It; ++It)
	{
		float x = It.Key().X;
		float y = It.Key().Y;

		auto xPlus = WorldSlices.Find(FVector2D(x + 1, y));
		auto xMinus = WorldSlices.Find(FVector2D(x - 1, y));
		if (xPlus)
			It.Value()->setNeighbour(NeighbourSlice::X_P, *xPlus);
		if (xMinus)
			It.Value()->setNeighbour(NeighbourSlice::X_M, *xMinus);

		auto yPlus = WorldSlices.Find(FVector2D(x, y + 1));
		auto yMinus = WorldSlices.Find(FVector2D(x, y - 1));
		if (yPlus)
			It.Value()->setNeighbour(NeighbourSlice::Y_P, *yPlus);
		if (yMinus)
			It.Value()->setNeighbour(NeighbourSlice::Y_M, *yMinus);
	}
}

void AInfinitiveWorldGenerator::continueGenerationProcess(SliceState gState, int range)
{
	for (int x = PlayerAtSlice.X - range; x <= PlayerAtSlice.X + range; x++)
		for (int y = PlayerAtSlice.Y - range; y <= PlayerAtSlice.Y + range; y++) {
			auto slice = WorldSlices.Find({ (float)x, (float)y });
			if (*slice) {
				(*slice)->continueGenerationProcess(gState);
			}
		}
}

void AInfinitiveWorldGenerator::renderMeshes(int range)
{
	for (int x = PlayerAtSlice.X - range; x <= PlayerAtSlice.X + range; x++)
		for (int y = PlayerAtSlice.Y - range; y <= PlayerAtSlice.Y + range; y++) {
			auto slice = WorldSlices.Find({ (float)x, (float)y });
			if (*slice) {
				if ((*slice)->getState() == Mesh) {
					(*slice)->drawMesh();
					return; // only draw one per frame.
				}
			}
		}
}

