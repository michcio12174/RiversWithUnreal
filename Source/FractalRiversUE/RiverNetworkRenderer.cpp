#include "RiverNetworkRenderer.h"

// Add default functionality here for any URiverNetworkRenderer functions that are not pure virtual.

 void URiverNetworkRenderer::generateAndRender(FVector2D origin, FVector2D target, int iterationDepth, int initialGenerator)
 {
 	this->initialGenerator = initialGenerator;

 	//calculate initial link length
 	initialLinkLength = (target - origin).Size() / 2;

 	//clear previous data and generate new
 	clear();
 	graph.generateNetwork(iterationDepth, initialGenerator);
 	FVector2D temp = target - origin;
 	temp.Normalize();
 	addLines(graph.root, temp, origin);
 }

 void URiverNetworkRenderer::clear()
 {
 	lines.Empty();
 	graph.resetGraph(initialGenerator);
 }

 void URiverNetworkRenderer::addLines(unique_ptr<link>& linkToAdd, FVector2D prevDirection, FVector2D originPoint)
 {
 	float x = prevDirection.X;
 	float y = prevDirection.Y;
 	FVector2D currentDirection = FVector2D(x * cos(linkToAdd->angleInRadians) - y * sin(linkToAdd->angleInRadians),
 		x * sin(linkToAdd->angleInRadians) + y * cos(linkToAdd->angleInRadians));
 	float currentLength = initialLinkLength * linkToAdd->length;
 	FVector2D endPoint = originPoint + currentDirection * currentLength;

 	//add recursively children of current line
 	if (linkToAdd->child1) addLines(linkToAdd->child1, currentDirection, endPoint);
 	if (linkToAdd->child2) addLines(linkToAdd->child2, currentDirection, endPoint);

 	//add line
	lines.Add(FLine2D(FVector2D(originPoint.X, originPoint.Y), FVector2D(endPoint.X, endPoint.Y)));
 }