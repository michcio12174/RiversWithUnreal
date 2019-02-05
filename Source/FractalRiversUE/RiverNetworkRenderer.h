// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "RiverGraph.h"

#include "CoreMinimal.h"
#include "RiverNetworkRenderer.generated.h"

USTRUCT(BlueprintType)
struct FLine2D {
	GENERATED_BODY()

	FLine2D(){}

	FLine2D(FVector2D p1, FVector2D p2):
		p1(p1), p2(p2) {}

	UPROPERTY(BlueprintReadWrite)
	FVector2D p1;
	UPROPERTY(BlueprintReadWrite)
	FVector2D p2;
};

UCLASS(BlueprintType)
 class FRACTALRIVERSUE_API URiverNetworkRenderer : public UObject {
 	GENERATED_BODY()

 public:
	 URiverNetworkRenderer() {};

 	UFUNCTION(BlueprintCallable)
 	void generateAndRender(FVector2D origin, FVector2D target, int iterationDepth, int initialGenerator);
 	UFUNCTION(BlueprintCallable)
 	void clear();

 	UPROPERTY(BlueprintReadWrite)
	TArray<FLine2D> lines;
 	UPROPERTY(BlueprintReadWrite)
 	int initialGenerator;

 private:
 	float initialLinkLength;
 	RiverGraph graph;

 	void addLines(unique_ptr<link> & linkToAdd, FVector2D prevDirection, FVector2D originPoint);
 };
