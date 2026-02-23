// Copyright 2025 Lauren Campbell (laurencc@usc.edu)

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "Engine/TriggerBox.h"
#include "TriggerBox_GameEffect.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ETriggerBoxType : uint8
{
	None,
	Capture,
	RedSpawn,
	BlueSpawn
};

UCLASS(Blueprintable)
class MULTI_API ATriggerBox_GameEffect : public ATriggerBox
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Abilities)
	TSubclassOf<class UGameplayEffect> GameplayEffect;

	UPROPERTY(Transient)
	TMap<class AShooterCharacter*, FActiveGameplayEffectHandle> GameplayEffectHandles;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Type of Effect")
	ETriggerBoxType Type = ETriggerBoxType::None;

	UFUNCTION()
	void HandleOnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void HandleOnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
