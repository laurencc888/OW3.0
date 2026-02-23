// Copyright 2025 Lauren Campbell (laurencc@usc.edu)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "ShooterGameState.generated.h"

/**
 * 
 */
UCLASS()
class MULTI_API AShooterGameState : public AGameState
{
	GENERATED_BODY()

public:
	AShooterGameState();
	
	UPROPERTY(Replicated)
	int RedScore;

	UPROPERTY(Replicated)
	int BlueScore;

	UPROPERTY(Replicated)
	float WaitingToStartTime = 0.0f;

	UPROPERTY(Replicated)
	int PlayersReady = 0;
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastAlert(const FString& Text, FLinearColor Color, float Duration);

	UFUNCTION(NetMulticast, Reliable)
	void PlaySequences();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void HandleMatchIsWaitingToStart() override;

	virtual void Tick(float DeltaSeconds) override;
};
