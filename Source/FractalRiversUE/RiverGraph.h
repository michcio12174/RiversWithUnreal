// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <random>
#include <memory>

using namespace std;

struct link //takes angle in degrees but stores in radians
{
	link(float angleInDegrees, float length, int HortonStrahlerNumber) :
		length(length),
		child1(nullptr),
		child2(nullptr),
		HortonStrahlerNumber(HortonStrahlerNumber)
	{
		//later down the line sin and cos from std library use radians
		//no need to store in degrees
		angleInRadians = angleInDegrees * 3.14159265358979323846f / 180.0f;
	}

	float angleInRadians;
	float length;
	int HortonStrahlerNumber;
	unique_ptr<link> child1;
	unique_ptr<link> child2;
};

class FRACTALRIVERSUE_API RiverGraph
{
public:
	unique_ptr<link> root;

	RiverGraph();

	void generateNetwork(int maxDepth, int initialGenerator);
	void resetGraph(int initialGenerator);

private:
	int currentIteration;
	int initialGenerator;
	float defaultAngle;
	//random infrastructure
	random_device randomDevice; //this engine will create a seed different evry time I run the program - exceptionally random but slow
	mt19937 mt; //this engine generates random numbers - vary fast and still very random
	uniform_int_distribution<int> distribution; //this will filter output of engine and will be the actual source of numbers

	void divideSegment(unique_ptr<link> & currentSegment);
	void replaceInternalLink(unique_ptr<link> & segmentToReplace, int generatorNumber, float length);
	void replaceExternalLink(unique_ptr<link> & segmentToReplace, int generatorNumber, float length);
	void updateHS(unique_ptr<link> & currentSegment);
};
