// Copyright 2025 Lauren Campbell (laurencc@usc.edu)


#include "TriggerBox_GameEffect.h"

#include "AbilitySystemComponent.h"
#include "ControlGameState.h"
#include "GameplayEffectTypes.h"
#include "ShooterCharacter.h"
#include "Components/ShapeComponent.h"

void ATriggerBox_GameEffect::BeginPlay()
{
	Super::BeginPlay();
	
	GetCollisionComponent()->OnComponentBeginOverlap.AddDynamic(this, &ATriggerBox_GameEffect::HandleOnBeginOverlap);
	GetCollisionComponent()->OnComponentEndOverlap.AddDynamic(this, &ATriggerBox_GameEffect::HandleOnEndOverlap);
}

void ATriggerBox_GameEffect::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetCollisionComponent()->OnComponentBeginOverlap.RemoveAll(this);
	GetCollisionComponent()->OnComponentEndOverlap.RemoveAll(this);
}

void ATriggerBox_GameEffect::HandleOnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}
	
	if (AShooterCharacter* Character = Cast<AShooterCharacter>(OtherActor))
	{
		// applying effect
		FGameplayEffectContextHandle ContextHandle;
		if (AShooterPlayerState* PS = Character->GetPlayerState<AShooterPlayerState>())
		{
			GameplayEffectHandles.Emplace(Character, PS->GetAbilitySystemComponent()->BP_ApplyGameplayEffectToSelf(GameplayEffect, 0.0f, ContextHandle));

			if (Type == ETriggerBoxType::Capture)
			{
				if (AControlGameState* GameState = GetWorld()->GetGameState<AControlGameState>())
				{
					GameState->OnEnterPoint(Character);
				}
			}
		}
	}
}

void ATriggerBox_GameEffect::HandleOnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!HasAuthority())
	{
		return;
	}
	
	if (AShooterCharacter* Character = Cast<AShooterCharacter>(OtherActor))
	{
		if (GameplayEffectHandles.Contains(Character))
		{
			// removing effect
			FActiveGameplayEffectHandle EffectHandle = GameplayEffectHandles[Character];

			// make sure player is alive..
			if (AShooterPlayerState* PS = Character->GetPlayerState<AShooterPlayerState>())
			{
				if (UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent())
				{
					ASC->RemoveActiveGameplayEffect(EffectHandle);
				}
			}
			EffectHandle.Invalidate();
			GameplayEffectHandles.Remove(Character);

			if (Type == ETriggerBoxType::Capture)
			{
				if (AControlGameState* GameState = GetWorld()->GetGameState<AControlGameState>())
				{
					GameState->OnLeavePoint(Character);
				}
			}
		}
	}
}
