// Copyright 2025 Lauren Campbell (laurencc@usc.edu)


#include "ControlGameMode.h"

#include "ControlGameInstanceSubsystem.h"
#include "ControlGameState.h"
#include "Engine/PlayerStartPIE.h"
#include "EngineUtils.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "ShooterCharacter.h"
#include "ShooterPlayerController.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void AControlGameMode::GenericPlayerInitialization(AController* C)
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

bool AControlGameMode::ShouldSpawnAtStartSpot(AController* Player)
{
	return false;
}

AActor* AControlGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	// Choose a player start
	APlayerStart* FoundPlayerStart = nullptr;
	UClass* PawnClass = GetDefaultPawnClassForController(Player);
	APawn* PawnToFit = PawnClass ? PawnClass->GetDefaultObject<APawn>() : nullptr;
	TArray<APlayerStart*> UnOccupiedStartPoints;
	TArray<APlayerStart*> OccupiedStartPoints;
	TArray<APlayerStart*> BackupStartPoints;
	UWorld* World = GetWorld();

	Players.Add(Cast<APlayerController>(Player));

	// NEW!!!! Figure out the player's team
	EShooterTeam Team = Player->GetPlayerState<AShooterPlayerState>()->Team;

	// NEW!!!! Assign start tag based on team
	FName PlayerStartTag = NAME_None;
	switch (Team)
	{
	case EShooterTeam::None: // default to red team
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

bool AControlGameMode::ReadyToEndMatch_Implementation()
{
	if (Super::ReadyToEndMatch_Implementation())
	{
		return true;
	}

	// end at 100%
	if (AControlGameState* GS = GetGameState<AControlGameState>())
	{
		if (GS->GetRedPercent() >= 100 || GS->GetBluePercent() >= 100)
		{
			return true;
		}
	}
	return false;
}

void AControlGameMode::HandleMatchHasEnded()
{
	if (AControlGameState* GS = GetGameState<AControlGameState>())
	{
		// preserve team points across rounds
		UControlGameInstanceSubsystem* GI = GetGameInstance()->GetSubsystem<UControlGameInstanceSubsystem>();
		
		if (GS->GetRedPercent() >= 100)
		{
			GI->IncrementRedWins();
			GS->IncreaseRedPts();
			
			if (GI->GetRedWins() == 1)
			{
				GS->MulticastControlAlert(TEXT("RED TEAM WINS THE ROUND!"), FLinearColor::Red, 5.0f);
			}
			else // red wins best of 3
			{
				GS->MulticastControlAlert(TEXT("RED TEAM WINS!"), FLinearColor::Red, 5.0f);
			}
		}
		else if (GS->GetBluePercent() >= 100)
		{
			GI->IncrementBlueWins();
			GS->IncreaseBluePts();
			
			if (GI->GetBlueWins() == 1)
			{
				GS->MulticastControlAlert(TEXT("BLUE TEAM WINS THE ROUND!"), FLinearColor::Blue, 5.0f);
			}
			else // blue wins best of 3
			{
				GS->MulticastControlAlert(TEXT("BLUE TEAM WINS!"), FLinearColor::Blue, 5.0f);
			}
		}
		
		// restart after 5 seconds
		GS->SetMatchState(MatchState::WaitingPostMatch);
		WaitingPostMatchDuration = 5.0f;
	}
}

void AControlGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	FName MS = GetMatchState();
	if (MS == MatchState::WaitingPostMatch)
	{
		WaitingPostMatchDuration -= DeltaSeconds;
		if (WaitingPostMatchDuration <= 0.0f)
		{
			WaitingPostMatchDuration = 5.0f;
			
			if (AControlGameState* GameState = GetGameState<AControlGameState>())
			{
				// preserving round pts
				UControlGameInstanceSubsystem* GI = GetGameInstance()->GetSubsystem<UControlGameInstanceSubsystem>();
				int TempRedPts = GI->GetRedWins();
				int TempBluePts = GI->GetBlueWins();

				// game over
				if (TempRedPts == 2 || TempBluePts == 2)
				{
					GI->ResetWins();
				}
				
				GameState->WaitingToStartTime = 5.0f;
				WaitingToStartDuration = 5.0f;
				GameState->SetMatchState(MatchState::WaitingToStart);

				RestartGame();
			}
		}
	}
}

void AControlGameMode::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();
}

void AControlGameMode::InitGameState()
{
	Super::InitGameState();

	// preserve round wins
	if (UControlGameInstanceSubsystem* GI = GetGameInstance()->GetSubsystem<UControlGameInstanceSubsystem>())
	{
		if (AControlGameState* GS = GetGameState<AControlGameState>())
		{
			GS->SetRoundWins(GI->GetRedWins(), GI->GetBlueWins());
		}
	}
}

void AControlGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AControlGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

AControlGameMode::AControlGameMode()
{
	PlayerStateClass = AShooterPlayerState::StaticClass();
	GameStateClass = AControlGameState::StaticClass();

	bDelayedStart = true;
	
	TeamSizes.Add(EShooterTeam::Red, 0);
	TeamSizes.Add(EShooterTeam::Blue, 0);
}