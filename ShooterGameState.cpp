// Copyright 2025 Lauren Campbell (laurencc@usc.edu)


#include "ShooterGameState.h"

#include "ControlGameMode.h"
#include "LevelSequenceActor.h"
#include "ShooterBPLibrary.h"
#include "ShooterGameMode.h"
#include "ShooterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "LevelSequencePlayer.h"

AShooterGameState::AShooterGameState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AShooterGameState::PlaySequences_Implementation()
{
	TArray<AActor*> Seqs;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), ALevelSequenceActor::StaticClass(), FName("SEQ"), Seqs);
    
	for (AActor* Seq : Seqs)
	{
		if (ALevelSequenceActor* SeqActor = Cast<ALevelSequenceActor>(Seq))
		{
			if (ULevelSequencePlayer* SeqPlay = SeqActor->GetSequencePlayer())
			{
				SeqPlay->Play();
			}
		}
	}
}

void AShooterGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterGameState, RedScore);
	DOREPLIFETIME(AShooterGameState, BlueScore);
	
	// Only send this on the "initial bunch" when the client first gets this actor replicated
	DOREPLIFETIME_CONDITION(AShooterGameState, WaitingToStartTime, COND_InitialOnly);
	
	DOREPLIFETIME(AShooterGameState, PlayersReady);
}

void AShooterGameState::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();

	// If we're on the server, set the waiting to start time
	if (GetLocalRole() == ROLE_Authority)
	{
		if (const AShooterGameMode* ShooterGM = GetDefaultGameMode<AShooterGameMode>())
		{
			WaitingToStartTime = ShooterGM->WaitingToStartDuration;
		}
		else if (const AControlGameMode* ControlGM = GetDefaultGameMode<AControlGameMode>())
		{
			WaitingToStartTime = ControlGM->WaitingToStartDuration;
		}
		// WaitingToStartTime = GetDefaultGameMode<AShooterGameMode>()->WaitingToStartDuration;
	}
}

void AShooterGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetMatchState() == MatchState::WaitingToStart && PlayerArray.Num() > 0 && PlayerArray.Num() == PlayersReady)
	{
		// should also start sequence countdowns!
		
		
		WaitingToStartTime -= DeltaSeconds;
		if (WaitingToStartTime <= 0.0f)
		{
			WaitingToStartTime = 0.0f;

			PlaySequences();
			
			// On server, actually start the match!
			if (GetLocalRole() == ROLE_Authority)
			{
				if (AShooterGameMode* GameMode = UShooterBPLibrary::GetShooterGameMode(this))
				{
					GameMode->StartMatch();
					MulticastAlert(TEXT("STARTING MATCH"), FLinearColor::Green, 3.0f);
				}
				else if (AControlGameMode* ControlGameMode = UShooterBPLibrary::GetControlGameMode(this))
				{
					ControlGameMode->StartMatch();
					MulticastAlert(TEXT("STARTING MATCH"), FLinearColor::Green, 3.0f);
				}
			}
		}
	}
}

void AShooterGameState::MulticastAlert_Implementation(const FString& Text, FLinearColor Color, float Duration)
{
	if (AShooterPlayerController* Controller = UShooterBPLibrary::GetShooterController(GetWorld(), 0))
	{
		if (Controller->IsLocalController())
		{
			Controller->OnAlert(Text, Color, Duration);
		}
	}
}
