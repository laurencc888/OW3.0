// Copyright 2025 Lauren Campbell (laurencc@usc.edu)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "ControlGameMode.generated.h"

enum class EShooterTeam : uint8;
/**
 * 
 */
UCLASS(abstract)
class MULTI_API AControlGameMode : public AGameMode
{
	GENERATED_BODY()

protected:

	/** Map of scores by team ID */
	TMap<uint8, int32> TeamScores;

	// map of number of players per team
	TMap<EShooterTeam, int> TeamSizes;

	// use same one from shooter game mode
	virtual void GenericPlayerInitialization(AController* C) override;

	virtual bool ShouldSpawnAtStartSpot(AController* Player) override;
	
	// make them spawn in their respective spawns
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	// if either team has reached 100%
	virtual bool ReadyToEndMatch_Implementation() override;

	virtual void HandleMatchHasEnded() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void HandleMatchIsWaitingToStart() override;

	virtual void InitGameState() override;

protected:

	/** Gameplay initialization */
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	AControlGameMode();
	
	// Time (in seconds) before the match will start
	UPROPERTY(EditDefaultsOnly)
	float WaitingToStartDuration = 5.0f;

	float WaitingPostMatchDuration = 5.0f;

private:
	float WaitingToAutoSaveDuration = 5.0f;

	float ShowLoadCircleDuration = 0.25f;

	bool bStartCircleCooldown = false;

	UPROPERTY()
	TArray<class APlayerController*> Players;
};
