// Copyright 2025 Lauren Campbell (laurencc@usc.edu)

#pragma once

#include "CoreMinimal.h"
#include "MultiSaveGame.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MultiSaveSystem.generated.h"

/**
 * 
 */
UCLASS()
class MULTI_API UMultiSaveSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<UMultiSaveGame> GroundTruth;

	void SaveGame(bool bIsAsync);
	
	void LoadGame();

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable)
	void NewSave();

	UFUNCTION(BlueprintCallable)
	bool HasSavedGame();
	
	UFUNCTION(BlueprintCallable)
	void NewControlSave();
};
