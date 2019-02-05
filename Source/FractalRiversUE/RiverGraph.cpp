// Fill out your copyright notice in the Description page of Project Settings.

#include "RiverGraph.h"

RiverGraph::RiverGraph() :
	initialGenerator(0),
	currentIteration(0),
	defaultAngle(30),
	mt(mt19937(randomDevice())),
	distribution(uniform_int_distribution <int>(0, 999))
{
}

void RiverGraph::generateNetwork(int maxDepth, int initialGenerator)
{
	resetGraph(initialGenerator);

	mt = mt19937(randomDevice());

	for (currentIteration = 2; currentIteration < maxDepth + 1; ++currentIteration) {
		divideSegment(root);
	}

	updateHS(root);
}

//performs one iteration step at the time recursively for whole tree
void RiverGraph::divideSegment(unique_ptr<link> & currentSegment)
{
	//recursively dividing children, so the tree will grow from external links to internal
	if (currentSegment->child1) divideSegment(currentSegment->child1);
	if (currentSegment->child2) divideSegment(currentSegment->child2);

	//the length schould be computed here after we choose the generator
	//wygląda na to, że obliczane l to długości linków w obecnej iteracji względem pierwszej iteracji
	float P = (1 + sqrt(1 + 4 * initialGenerator)) / (2 * initialGenerator);
	float l = pow(1.0f / (P * initialGenerator), currentIteration - 1);

	if (currentSegment->child1 || currentSegment->child2) {
		//link is internal
		float R = P * (initialGenerator - 1);
		float fractionalR, integralR;
		fractionalR = modf(R, &integralR); //returns fractional part of number
		int probability = (int)(fractionalR * 1000.0f);

		//rolling the dice
		int temp = distribution(mt);
		bool result = temp < probability; //make distribution uniform

		/*string message = " probability: ";
		message += to_string(probability) + " distribution: " + to_string(temp) + " go for 2: " + to_string(result);
		Logger::writeToLog(message);*/

		if (result) replaceInternalLink(currentSegment, 2, l);
		else replaceInternalLink(currentSegment, 1, l);
	}
	else {
		//link is external
		replaceExternalLink(currentSegment, initialGenerator, l);
	}
}


//directions of tributaries are flipped in comparision to external links to create more natural look
void RiverGraph::replaceInternalLink(unique_ptr<link> & segmentToReplace, int generatorNumber, float length)
{
	//gets the children and replaces their values in unique_ptr with null
	unique_ptr<link> originalChild1(segmentToReplace->child1.release());
	unique_ptr<link> originalChild2(segmentToReplace->child2.release());
	segmentToReplace->length = length;

	switch (generatorNumber) {
	case 1:
		//  
		// - -
		// 

		//extend the river
		segmentToReplace->child1 = unique_ptr<link>(new link(0, length, 1));
		//set the original children
		segmentToReplace->child1->child1 = unique_ptr<link>(originalChild1.release());
		segmentToReplace->child1->child2 = unique_ptr<link>(originalChild2.release());

		break;
	case 2:
		//  
		// - -
		//  \ 

		//extend the river
		segmentToReplace->child1 = unique_ptr<link>(new link(0, length, 1));
		segmentToReplace->child2 = unique_ptr<link>(new link(-2 * defaultAngle, length, 1));
		//set the original children
		segmentToReplace->child1->child1 = unique_ptr<link>(originalChild1.release());
		segmentToReplace->child1->child2 = unique_ptr<link>(originalChild2.release());

		break;
	case 3:
		//    / 
		// - - -
		//  \ 

		//extend the river
		segmentToReplace->child1 = unique_ptr<link>(new link(0, length, 1));
		segmentToReplace->child2 = unique_ptr<link>(new link(-2 * defaultAngle, length, 1));
		segmentToReplace->child1->child1 = unique_ptr<link>(new link(0, length, 1));
		segmentToReplace->child1->child2 = unique_ptr<link>(new link(2 * defaultAngle, length, 1));
		//set the original children
		segmentToReplace->child1->child1->child1 = unique_ptr<link>(originalChild1.release());
		segmentToReplace->child1->child1->child2 = unique_ptr<link>(originalChild2.release());

		break;

	case 4:
		//    /
		// - - - -
		//  \   \ 

		//extend the river
		segmentToReplace->child1 = unique_ptr<link>(new link(0, length, 1));
		segmentToReplace->child2 = unique_ptr<link>(new link(-2 * defaultAngle, length, 1));
		segmentToReplace->child1->child1 = unique_ptr<link>(new link(0, length, 1));
		segmentToReplace->child1->child2 = unique_ptr<link>(new link(2 * defaultAngle, length, 1));
		segmentToReplace->child1->child1->child1 = unique_ptr<link>(new link(0, length, 1));
		segmentToReplace->child1->child1->child2 = unique_ptr<link>(new link(-2 * defaultAngle, length, 1));
		//set the original children
		segmentToReplace->child1->child1->child1->child1 = unique_ptr<link>(originalChild1.release());
		segmentToReplace->child1->child1->child1->child2 = unique_ptr<link>(originalChild2.release());

		break;
	}
}

void RiverGraph::replaceExternalLink(unique_ptr<link> & segmentToReplace, int generatorNumber, float length)
{
	segmentToReplace->length = length;

	//function needs to make sure horton-Strahler numbers of added links are right
	//they won't be evaluated in this iteration of divideSegment
	switch (generatorNumber) {
	case 1:
		//  / 
		// - 
		//  \ 
		segmentToReplace->child1 = unique_ptr<link>(new link(defaultAngle, length, 1));
		segmentToReplace->child2 = unique_ptr<link>(new link(-defaultAngle, length, 1));
		break;
	case 2:
		//    / 
		// - - 
		//  \ \ 
		segmentToReplace->child1 = unique_ptr<link>(new link(0, length, 2));//g³ówna rzeka
		segmentToReplace->child2 = unique_ptr<link>(new link(2 * defaultAngle, length, 1));//boczny dop³yw
		segmentToReplace->child1->child1 = unique_ptr<link>(new link(defaultAngle, length, 1));//koñcowe rozga³êzienie
		segmentToReplace->child1->child2 = unique_ptr<link>(new link(-defaultAngle, length, 1));
		break;
	case 3:
		//    / / 
		// - - - 
		//  \   \ 
		segmentToReplace->child1 = unique_ptr<link>(new link(0, length, 2));//g³ówna rzeka 1
		segmentToReplace->child2 = unique_ptr<link>(new link(2 * defaultAngle, length, 1));//boczny dop³yw 1
		segmentToReplace->child1->child1 = unique_ptr<link>(new link(0, length, 2));//g³ówna rzeka 2
		segmentToReplace->child1->child2 = unique_ptr<link>(new link(-2 * defaultAngle, length, 1));//boczny dop³yw 2
		segmentToReplace->child1->child1->child1 = unique_ptr<link>(new link(defaultAngle, length, 1));//koñcowe rozga³êzienie
		segmentToReplace->child1->child1->child2 = unique_ptr<link>(new link(-defaultAngle, length, 1));
		break;

	case 4:
		//    /   / 
		// - - - - 
		//  \   \ \ 
		segmentToReplace->child1 = unique_ptr<link>(new link(0, length, 2));//g³ówna rzeka 1
		segmentToReplace->child2 = unique_ptr<link>(new link(2 * defaultAngle, length, 1));//boczny dop³yw 1
		segmentToReplace->child1->child1 = unique_ptr<link>(new link(0, length, 2));//g³ówna rzeka 2
		segmentToReplace->child1->child2 = unique_ptr<link>(new link(-2 * defaultAngle, length, 1));//boczny dop³yw 2
		segmentToReplace->child1->child1->child1 = unique_ptr<link>(new link(0, length, 2));//g³ówna rzeka 3
		segmentToReplace->child1->child1->child2 = unique_ptr<link>(new link(2 * defaultAngle, length, 1));//boczny dop³yw 3
		segmentToReplace->child1->child1->child1->child1 = unique_ptr<link>(new link(defaultAngle, length, 1));//koñcowe rozga³êzienie
		segmentToReplace->child1->child1->child1->child2 = unique_ptr<link>(new link(-defaultAngle, length, 1));
		break;
	}
}

void RiverGraph::updateHS(unique_ptr<link> & currentSegment)
{
	if (currentSegment->child1) updateHS(currentSegment->child1);
	if (currentSegment->child2) updateHS(currentSegment->child2);

	//updating Horton-Strahler number
	int HS1 = 0, HS2 = 0;
	if (currentSegment->child1) HS1 = currentSegment->child1->HortonStrahlerNumber;
	if (currentSegment->child2) HS2 = currentSegment->child2->HortonStrahlerNumber;

	if (HS1 == HS2) currentSegment->HortonStrahlerNumber = ++HS1;
	else {
		int tempMax = max(HS1, HS2);
		currentSegment->HortonStrahlerNumber = tempMax;
	}
}

void RiverGraph::resetGraph(int initialGenerator)
{
	this->initialGenerator = initialGenerator;

	root.reset(nullptr);
	root = unique_ptr<link>(new link(0, 1, 2));

	//creation of the root
	switch (initialGenerator) {
	case 3:
		root->child1 = unique_ptr<link>(new link(0, 1, 2));//g³ówna rzeka 1
		root->child2 = unique_ptr<link>(new link(2 * defaultAngle, 1, 1));//boczny dop³yw 1
		root->child1->child1 = unique_ptr<link>(new link(0, 1, 2));//g³ówna rzeka 2
		root->child1->child2 = unique_ptr<link>(new link(-2 * defaultAngle, 1, 1));//boczny dop³yw 2
		root->child1->child1->child1 = unique_ptr<link>(new link(defaultAngle, 1, 1));//koñcowe rozga³êzienie
		root->child1->child1->child2 = unique_ptr<link>(new link(-defaultAngle, 1, 1));
		break;

	case 4:
		root->child1 = unique_ptr<link>(new link(0, 1, 2));//g³ówna rzeka 1
		root->child2 = unique_ptr<link>(new link(2 * defaultAngle, 1, 1));//boczny dop³yw 1
		root->child1->child1 = unique_ptr<link>(new link(0, 1, 2));//g³ówna rzeka 2
		root->child1->child2 = unique_ptr<link>(new link(-2 * defaultAngle, 1, 1));//boczny dop³yw 2
		root->child1->child1->child1 = unique_ptr<link>(new link(0, 1, 2));//g³ówna rzeka 3
		root->child1->child1->child2 = unique_ptr<link>(new link(2 * defaultAngle, 1, 1));//boczny dop³yw 3
		root->child1->child1->child1->child1 = unique_ptr<link>(new link(defaultAngle, 1, 1));//koñcowe rozga³êzienie
		root->child1->child1->child1->child2 = unique_ptr<link>(new link(-defaultAngle, 1, 1));
		break;

	default:
		root->child1 = unique_ptr<link>(new link(0, 1, 2));//przed³u¿enie w linii prostej
		root->child2 = unique_ptr<link>(new link(2 * defaultAngle, 1, 1));//odp³yw z boku
		root->child1->child1 = unique_ptr<link>(new link(defaultAngle, 1, 1));//dwa odp³ywy na koñcu
		root->child1->child2 = unique_ptr<link>(new link(-defaultAngle, 1, 1));
		break;
	}

}
