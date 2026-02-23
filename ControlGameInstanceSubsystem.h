// Copyright 2025 Lauren Campbell (laurencc@usc.edu)

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ControlGameInstanceSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class MULTI_API UControlGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	int RedRoundWins = 0;
	int BlueRoundWins = 0;

	void IncrementRedWins() { RedRoundWins++; }
	void IncrementBlueWins() { BlueRoundWins++; }
	void ResetWins() { RedRoundWins = 0; BlueRoundWins = 0; }
    
	int GetRedWins() const { return RedRoundWins; }
	int GetBlueWins() const { return BlueRoundWins; }
};
