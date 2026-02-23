// Copyright Epic Games, Inc. All Rights Reserved.


#include "Variant_Shooter/ShooterGameMode.h"

#include "ShooterGameState.h"
#include "Engine/PlayerStartPIE.h"
#include "EngineUtils.h"
#include "MultiSaveSystem.h"
#include "ShooterBPLibrary.h"
#include "ShooterBulletCounterUI.h"
#include "ShooterCharacter.h"
#include "ShooterPlayerController.h"
#include "Engine/World.h"

void AShooterGameMode::GenericPlayerInitialization(AController* C)
{
	Super::GenericPlayerInitialization(C);

	// check if player is already assigned to a team
	if (AShooterPlayerState* PlayerState = C->GetPlayerState<AShooterPlayerState>())
	{
		EShooterTeam CurrTeam = PlayerState->Team;

		// can assume that there will only be two teams
		if (CurrTeam == EShooterTeam::None)
		{
			if (TeamSizes[EShooterTeam::Blue] < TeamSizes[EShooterTeam::Red])
			{
				PlayerState->Team = EShooterTeam::Blue;
				TeamSizes[EShooterTeam::Blue]++;
			}
			else
			{
				PlayerState->Team = EShooterTeam::Red;
				TeamSizes[EShooterTeam::Red]++;
			}
		}
	}
}

bool AShooterGameMode::ShouldSpawnAtStartSpot(AController* Player)
{
	return false;
}

AActor* AShooterGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	// Choose a player start
	APlayerStart* FoundPlayerStart = nullptr;
	UClass* PawnClass = GetDefaultPawnClassForController(Player);
	APawn* PawnToFit = PawnClass ? PawnClass->GetDefaultObject<APawn>() : nullptr;
	TArray<APlayerStart*> UnOccupiedStartPoints;
	TArray<APlayerStart*> OccupiedStartPoints;
	TArray<APlayerStart*> BackupStartPoints;
	UWorld* World = GetWorld();

	// NEW!!!! Figure out the player's team
	EShooterTeam Team = Player->GetPlayerState<AShooterPlayerState>()->Team;

	// NEW!!!! Assign start tag based on team
	FName PlayerStartTag = NAME_None;
	switch (Team)
	{
	case EShooterTeam::None:
		PlayerStartTag = "Backup";
		break;
	case EShooterTeam::Red:
		PlayerStartTag = "Red";
		break;
	case EShooterTeam::Blue:
		PlayerStartTag = "Blue";
		break;
	}
	
	for (TActorIterator<APlayerStart> It(World); It; ++It)
	{
		APlayerStart* PlayerStart = *It;

		if (PlayerStart->IsA<APlayerStartPIE>())
		{
			// Always prefer the first "Play from Here" PlayerStart, if we find one while in PIE mode
			FoundPlayerStart = PlayerStart;
			break;
		}
		// NEW!!!! Prioritize player starts which match the team
		else if (PlayerStartTag == NAME_None || PlayerStartTag == PlayerStart->PlayerStartTag)
		{
			FVector ActorLocation = PlayerStart->GetActorLocation();
			const FRotator ActorRotation = PlayerStart->GetActorRotation();
			if (!World->EncroachingBlockingGeometry(PawnToFit, ActorLocation, ActorRotation))
			{
				UnOccupiedStartPoints.Add(PlayerStart);
			}
			else if (World->FindTeleportSpot(PawnToFit, ActorLocation, ActorRotation))
			{
				OccupiedStartPoints.Add(PlayerStart);
			}
		}
		// add backup player starts to appropriate array
		if (PlayerStart->PlayerStartTag == "Backup")
		{
			BackupStartPoints.Add(PlayerStart);
		}
	}
	
	// remove player starts that are within 1000 units of an opposing player
	for (TActorIterator<AShooterCharacter> It(World); It; ++It)
	{
		AShooterCharacter* OtherChar = *It;
		EShooterTeam OtherTeam = OtherChar->Team;

		// check if it's an enemy player
		if (OtherTeam != Team)
		{
			// remove all start points that are near an enemy plater
			UnOccupiedStartPoints.RemoveAll([OtherChar](APlayerStart* PS)
			{
				return FVector::Distance(OtherChar->GetActorLocation(), PS->GetActorLocation()) < 1000.0f;
			});
			OccupiedStartPoints.RemoveAll([OtherChar](APlayerStart* PS)
			{
				return FVector::Distance(OtherChar->GetActorLocation(), PS->GetActorLocation()) < 1000.0f;
			});
		}
	}
		
	if (FoundPlayerStart == nullptr)
	{
		if (UnOccupiedStartPoints.Num() > 0)
		{
			FoundPlayerStart = UnOccupiedStartPoints[FMath::RandRange(0, UnOccupiedStartPoints.Num() - 1)];
		}
		else if (OccupiedStartPoints.Num() > 0)
		{
			FoundPlayerStart = OccupiedStartPoints[FMath::RandRange(0, OccupiedStartPoints.Num() - 1)];
		}
		else // if both arrays are empty, pick from backup
		{
			FoundPlayerStart = BackupStartPoints[FMath::RandRange(0, BackupStartPoints.Num() - 1)];
		}
	}
	return FoundPlayerStart;
}

bool AShooterGameMode::ReadyToEndMatch_Implementation()
{
	if (AShooterGameState* GameState = GetGameState<AShooterGameState>())
	{
		// if either team reaches 10 points, end the game
		if (GameState->RedScore == 10 || GameState->BlueScore == 10)
		{
			return true;
		}
	}
	return false;
}

void AShooterGameMode::HandleMatchHasEnded()
{
	if (AShooterGameState* GameState = GetGameState<AShooterGameState>())
	{
		if (GameState->RedScore == 10)
		{
			GameState->MulticastAlert(TEXT("RED TEAM WINS"), FLinearColor::Red, 5.0f);
		}
		else
		{
			GameState->MulticastAlert(TEXT("BLUE TEAM WINS"), FLinearColor::Blue, 5.0f);
		}
		// need to wait 5 seconds before restarting match
		GameState->SetMatchState(MatchState::WaitingPostMatch);
		WaitingPostMatchDuration = 5.0f;
	}
	if (UMultiSaveSystem* SaveSystem = GetGameInstance()->GetSubsystem<UMultiSaveSystem>())
	{
		SaveSystem->SaveGame(false);
	}
}

void AShooterGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	FName MS = GetMatchState();
	if (MS == MatchState::WaitingPostMatch)
	{
		WaitingPostMatchDuration -= DeltaSeconds;
		if (WaitingPostMatchDuration <= 0.0f)
		{
			WaitingPostMatchDuration = 5.0f;
			
			if (AShooterGameState* GameState = GetGameState<AShooterGameState>())
			{
				GameState->WaitingToStartTime = 5.0f;
				WaitingToStartDuration = 5.0f;
				GameState->SetMatchState(MatchState::WaitingToStart);
				RestartGame();
			}
		}
	}
	// need to autosave every 5 seconds when game is in progress
	else if (MS == MatchState::InProgress)
	{
		WaitingToAutoSaveDuration -= DeltaSeconds;
		if (WaitingToAutoSaveDuration <= 0.0f)
		{
			WaitingToAutoSaveDuration = 5.0f;

			if (UMultiSaveSystem* SaveSystem = GetGameInstance()->GetSubsystem<UMultiSaveSystem>())
			{
				SaveSystem->SaveGame(true);

				if (AShooterPlayerController* Controller = UShooterBPLibrary::GetShooterController(GetWorld(), 0))
				{
					Controller->GetBulletCounterUI()->ToggleLoadingCircle(true);
				}
			}
		}
		// setting duration for spinning circle
		if (bStartCircleCooldown)
		{
			ShowLoadCircleDuration -= DeltaSeconds;

			if (ShowLoadCircleDuration <= 0.0f)
			{
				if (AShooterPlayerController* Controller = UShooterBPLibrary::GetShooterController(GetWorld(), 0))
				{
					Controller->GetBulletCounterUI()->ToggleLoadingCircle(false);
				}
				bStartCircleCooldown = false;
			}
		}
	}
}

void AShooterGameMode::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();

	if (UMultiSaveSystem* SaveSystem = GetGameInstance()->GetSubsystem<UMultiSaveSystem>())
	{
		SaveSystem->LoadGame();
	}
}

void AShooterGameMode::BeginPlay()
{
	Super::BeginPlay();

}

void AShooterGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (UMultiSaveSystem* SaveSystem = GetGameInstance()->GetSubsystem<UMultiSaveSystem>())
	{
		SaveSystem->SaveGame(false);
	}
}

AShooterGameMode::AShooterGameMode()
{
	PlayerStateClass = AShooterPlayerState::StaticClass();
	GameStateClass = AShooterGameState::StaticClass();

	bDelayedStart = true;
	
	TeamSizes.Add(EShooterTeam::Red, 0);
	TeamSizes.Add(EShooterTeam::Blue, 0);
}

void AShooterGameMode::IncrementTeamScore(uint8 TeamByte)
{
	// retrieve the team score if any
	int32 Score = 0;
	if (int32* FoundScore = TeamScores.Find(TeamByte))
	{
		Score = *FoundScore;
	}

	// increment the score for the given team
	++Score;
	TeamScores.Add(TeamByte, Score);
}

void AShooterGameMode::TriggerCircleCooldown()
{
	bStartCircleCooldown = true;
	ShowLoadCircleDuration = 0.25f;
}
