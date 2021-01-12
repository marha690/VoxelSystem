// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class VOXELSYSTEM_API Generator_1
{
public:
	Generator_1();
	~Generator_1();

	int heightMap(int x, int y);
	bool makeTree(int x, int y);
	int chaosHash(int x, int y, int seed);

private:
};
