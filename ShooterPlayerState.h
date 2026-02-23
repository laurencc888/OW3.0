// Copyright 2025 Lauren Campbell (laurencc@usc.edu)

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "ShooterPlayerState.generated.h"

UENUM(BlueprintType)
enum class EShooterTeam : uint8
{
	/* Team not assigned yet*/
	None,
	/* On red team */
	Red,
	/* On blue team */
	Blue
};

/**
 * 
 */
UCLASS()
class MULTI_API AShooterPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AShooterPlayerState();
	
	UPROPERTY(Replicated)
	EShooterTeam Team = EShooterTeam::None;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void IncreaseStreak();
	void ResetStreak(FString Killer);

	UPROPERTY(EditDefaultsOnly)
	TMap<int, USoundBase*> StreakSounds;

	UPROPERTY(EditDefaultsOnly)
	TMap<int, FString> StreakMessages;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastAnnounceStreak(int Streak);

	UFUNCTION(Server, Reliable)
	void ServerSendMessage(EShooterTeam TargetTeam, const FString& Sender, const FString& Msg);

	UFUNCTION(Server, Reliable)
	void ServerSetReady(bool bIsReady);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Abilities)
	TObjectPtr<class UControlAbilitySystemComponent> AbilitySystemComponent;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void BeginPlay() override;

private:
	int CurrentStreak = 0;

	void StreakAlert(FString Msg, FLinearColor Color);
};
