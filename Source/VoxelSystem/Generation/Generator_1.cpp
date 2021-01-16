// Fill out your copyright notice in the Description page of Project Settings.


#include "Generator_1.h"

//Noise 
#include "../Noise/cellular.h"
#include "../Noise/noise1234.h"
#include "../Noise/simplexnoise1234.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctime>

Generator_1::Generator_1(int cSize)
{
	chunkSize = cSize;
}

Generator_1::~Generator_1()
{
}

int Generator_1::heightMap(int x, int y)
{
	float scale = 0.002f;
	auto val = (snoise2(x * scale, y * scale) + 1) / 2;
	auto val2 = (snoise2(x * scale / 2, y * scale / 2) + 1) / 2;

	auto height1 = (snoise2(x * scale, y * scale) + 1) / 2;
	auto height2 = (snoise2(x * scale * 2, y * scale * 2) + 1) / 2;
	auto height3 = (snoise2(x * scale * 4, y * scale * 4) + 1) / 2;
	auto result = 1 * height1 + 0.5 * height2 + 0.25 * height3;
	result = pow(result, 2.0);

	result = result * 50;

	return (int)result + 1;
}

bool Generator_1::makeTree(int x, int y)
{
	int mapSize = 64;
	int bX = abs(x) % mapSize;
	int bY = abs(y) % mapSize;
	int index = bX * mapSize + bY;
	uint8_t val = bigBlueNoise_map[index];

	if (val ==  233) {
		int h = heightMap(x, y);
		return (h < 30)? true: false;
	}
	return false;
}

int Generator_1::chaosHash(int x, int y, int seed)
{
	int h = seed + x * 374761393 + y * 668265263;
	h = (h ^ (h >> 13)) * 1274126177;
	return h ^ (h >> 16);
}

int Generator_1::rand(int range, int seed)
{
	srand(seed);
	int result = 1 + (std::rand() % range);
	return result;
}
