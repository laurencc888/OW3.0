// Copyright 2025 Lauren Campbell (laurencc@usc.edu)


#include "ShooterPlayerState.h"

#include "ControlAbilitySystemComponent.h"
#include "ControlGameState.h"
#include "Multi.h"
#include "ShooterBPLibrary.h"
#include "ShooterBulletCounterUI.h"
#include "ShooterGameState.h"
#include "ShooterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


AShooterPlayerState::AShooterPlayerState()
{
	// still experienced lag using 4&5
	SetNetUpdateFrequency(6);

	AbilitySystemComponent = CreateDefaultSubobject<UControlAbilitySystemComponent>(TEXT("AbilitySystem"));
}

void AShooterPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterPlayerState, Team);
}

void AShooterPlayerState::IncreaseStreak()
{
	CurrentStreak++;

	// send out an alert for killstreak
	if (FString* Msg = StreakMessages.Find(CurrentStreak))
	{
		FLinearColor Color = (Team == EShooterTeam::Red) ? FLinearColor::Red : FLinearColor::Blue;
		StreakAlert(GetPlayerName() + *Msg, Color);
		MulticastAnnounceStreak(CurrentStreak);
	}
}

void AShooterPlayerState::ResetStreak(FString Killer)
{
	if (CurrentStreak >= 3)
	{
		FString Msg = Killer + " ended " + GetPlayerName() + "'s streak!";
		FLinearColor Color = (Team == EShooterTeam::Red) ? FLinearColor::Blue : FLinearColor::Red;
		StreakAlert(Msg, Color);
	}
	CurrentStreak = 0;
}

void AShooterPlayerState::MulticastAnnounceStreak_Implementation(int Streak)
{
	if (USoundBase** Sound = StreakSounds.Find(Streak))
	{
		UGameplayStatics::PlaySound2D(GetWorld(), *Sound);
	}
}

void AShooterPlayerState::ServerSendMessage_Implementation(EShooterTeam TargetTeam, const FString& Sender,
	const FString& Msg)
{
	AShooterGameState* GS = UShooterBPLibrary::GetShooterGameState(GetWorld());

	// finding who to forward the message to
	for (auto PlayerState : GS->PlayerArray)
	{
		AShooterPlayerState* SPS = Cast<AShooterPlayerState>(PlayerState);
		if (TargetTeam == EShooterTeam::None || (TargetTeam == SPS->Team))
		{
			if (AShooterPlayerController* PC = SPS->GetOwner<AShooterPlayerController>())
			{
				PC->ClientReceiveMessage(TargetTeam, Sender, Msg);
			}
		}
	}
}

void AShooterPlayerState::ServerSetReady_Implementation(bool bIsReady)
{
	if (AShooterGameState* ShooterGS = UShooterBPLibrary::GetShooterGameState(GetWorld()))
	{
		if (bIsReady)
		{
			ShooterGS->PlayersReady++;
		}
		else
		{
			ShooterGS->PlayersReady--;
		}
	}
}

UAbilitySystemComponent* AShooterPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AShooterPlayerState::BeginPlay()
{
	Super::BeginPlay();
	
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void AShooterPlayerState::StreakAlert(FString Msg, FLinearColor Color)
{
	AShooterGameState* GameState = UShooterBPLibrary::GetShooterGameState(GetWorld());
	
	GameState->MulticastAlert(Msg, Color, 2.5f);
}
