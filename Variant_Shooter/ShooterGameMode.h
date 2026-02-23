// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ShooterPlayerState.h"
#include "GameFramework/GameMode.h"
#include "ShooterGameMode.generated.h"

class UShooterUI;

/**
 *  Simple GameMode for a first person shooter game
 *  Manages game UI
 *  Keeps track of team scores
 */
UCLASS(abstract)
class MULTI_API AShooterGameMode : public AGameMode
{
	GENERATED_BODY()
	
protected:

	/** Map of scores by team ID */
	TMap<uint8, int32> TeamScores;

	// map of number of players per team
	TMap<EShooterTeam, int> TeamSizes;

	virtual void GenericPlayerInitialization(AController* C) override;

	virtual bool ShouldSpawnAtStartSpot(AController* Player) override;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	virtual bool ReadyToEndMatch_Implementation() override;

	virtual void HandleMatchHasEnded() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void HandleMatchIsWaitingToStart() override;

protected:

	/** Gameplay initialization */
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	AShooterGameMode();

	/** Increases the score for the given team */
	void IncrementTeamScore(uint8 TeamByte);
	
	// Time (in seconds) before the match will start
	UPROPERTY(EditDefaultsOnly)
	float WaitingToStartDuration = 5.0f;

	float WaitingPostMatchDuration = 5.0f;

	void TriggerCircleCooldown();

private:
	float WaitingToAutoSaveDuration = 5.0f;

	float ShowLoadCircleDuration = 0.25f;

	bool bStartCircleCooldown = false;
};
