// Copyright 2025 Lauren Campbell (laurencc@usc.edu)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MultiSaveGame.generated.h"

USTRUCT()
struct FTagActorSaveData
{
	GENERATED_BODY()

public:
	// Save transform (position, rotation, scale) of tag
	UPROPERTY()
	FTransform Transform;

	// Type of TagActor this is
	UPROPERTY()
	TSubclassOf<class ATagActor> ClassType;
};

UCLASS()
class MULTI_API UMultiSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<FTagActorSaveData> TagActors;
};
