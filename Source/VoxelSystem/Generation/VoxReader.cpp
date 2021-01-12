// Fill out your copyright notice in the Description page of Project Settings.


#include "VoxReader.h"
#include "Misc/Paths.h"

// Basic file operations
#include <iostream>
#include <fstream>
#include <string>
#include <cstring> // for std::strlen
#include <cstddef> // for std::size_t -> is a typedef on an unsinged int


using namespace std;

VoxReader::VoxReader(FString FileName)
{
	auto contentDir = FPaths::ProjectContentDir();
	auto dataDir = FPaths::ConvertRelativePathToFull(contentDir, FolderName + FileName);
	ReadData(dataDir);
}

VoxReader::~VoxReader()
{
}


void VoxReader::ReadData(FString FilePath)
{
	char* y = new char[4];

	ifstream myFile(TCHAR_TO_UTF8(*FilePath), ios::in | ios::binary);
	myFile.seekg(0);
	verify(myFile.read((char*)y, sizeof(char) * 4));

	//.Vox file verification.
	int id = MV_ID((unsigned  char)(y[0]), (unsigned  char)(y[1]), (unsigned  char)(y[2]), (unsigned  char)(y[3]));
	if (id == ID_VOX) {

		myFile.read((char*)y, sizeof(BYTE) * 4);
		int version = MV_ID((unsigned  char)(y[0]), (unsigned  char)(y[1]), (unsigned  char)(y[2]), (unsigned  char)(y[3]));

		verify(version == 150); // is file magicavoxel v. 150?
	}

	//MAIN.
	verify(myFile.read((char*)y, sizeof(char) * 4));
	id = MV_ID((unsigned  char)(y[0]), (unsigned  char)(y[1]), (unsigned  char)(y[2]), (unsigned  char)(y[3]));
	if (id == ID_MAIN) {

		while (myFile.read((char*)y, sizeof(char) * 4)) {
			id = MV_ID((unsigned  char)(y[0]), (unsigned  char)(y[1]), (unsigned  char)(y[2]), (unsigned  char)(y[3]));

			// pack :optional
			if (id == ID_PACK) { //not suported!
				verify(false);
			}

			// size
			if (id == ID_SIZE) {
				myFile.read((char*)y, sizeof(char) * 4);
				int nByesInContent = MV_ID((unsigned  char)(y[0]), (unsigned  char)(y[1]), (unsigned  char)(y[2]), (unsigned  char)(y[3]));
				myFile.read((char*)y, sizeof(char) * 4);
				int nByesInChildren = MV_ID((unsigned  char)(y[0]), (unsigned  char)(y[1]), (unsigned  char)(y[2]), (unsigned  char)(y[3]));


				myFile.read((char*)y, sizeof(char) * 4);
				int xMax = MV_ID((unsigned  char)(y[0]), (unsigned  char)(y[1]), (unsigned  char)(y[2]), (unsigned  char)(y[3]));

				myFile.read((char*)y, sizeof(char) * 4);
				int yMax = MV_ID((unsigned  char)(y[0]), (unsigned  char)(y[1]), (unsigned  char)(y[2]), (unsigned  char)(y[3]));

				myFile.read((char*)y, sizeof(char) * 4);
				int zMax = MV_ID((unsigned  char)(y[0]), (unsigned  char)(y[1]), (unsigned  char)(y[2]), (unsigned  char)(y[3]));

				ModelSize = FVector(xMax, yMax, zMax);
			}

			// xyzi
			if (id == ID_XYZI) {
				myFile.read((char*)y, sizeof(char) * 4);
				int nByesInContent = MV_ID((unsigned  char)(y[0]), (unsigned  char)(y[1]), (unsigned  char)(y[2]), (unsigned  char)(y[3]));
				myFile.read((char*)y, sizeof(char) * 4);
				int nByesInChildren = MV_ID((unsigned  char)(y[0]), (unsigned  char)(y[1]), (unsigned  char)(y[2]), (unsigned  char)(y[3]));


				myFile.read((char*)y, sizeof(char) * 4);
				int nOfVoxels = MV_ID((unsigned  char)(y[0]), (unsigned  char)(y[1]), (unsigned  char)(y[2]), (unsigned  char)(y[3]));

				for (size_t i = 0; i < nOfVoxels; i++)
				{
					voxels.reserve(nOfVoxels);

					myFile.read((char*)y, sizeof(char));
					int yPos = (unsigned  char)(y[0]);
					myFile.read((char*)y, sizeof(char));
					int xPos = (unsigned  char)(y[0]);
					myFile.read((char*)y, sizeof(char));
					int zPos = (unsigned  char)(y[0]);
					myFile.read((char*)y, sizeof(char));
					int color = (unsigned  char)(y[0]);

					std::pair<FVector, int> vox{ FVector(xPos, yPos, zPos), color };
					voxels.push_back(vox);
				}

			}

			// palette

			// materials


		}
	}


	myFile.close();
}

std::string VoxReader::ConvertToString(char* a, int size)
{
	int i;
	string s = "";
	for (i = 0; i < size; i++) {
		s = s + a[i];
	}
	return s;
}
