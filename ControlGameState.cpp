// Copyright 2025 Lauren Campbell (laurencc@usc.edu)

#include "ControlGameState.h"

#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "ShooterBPLibrary.h"
#include "ShooterBulletCounterUI.h"
#include "ShooterCharacter.h"
#include "ShooterPlayerController.h"
#include "ShooterPlayerState.h"
#include "GameFramework/GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// referenced a shootergamestate for a lot of this
AControlGameState::AControlGameState()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AControlGameState::BeginPlay()
{
    Super::BeginPlay();
    
    if (HasAuthority())
    {
        CapturePercentage = 0;
        ControllingTeam = EShooterTeam::None;
        RedPercent = 0;
        BluePercent = 0;
    }
}

void AControlGameState::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // update server only
    if (!HasAuthority())
    {
        return;
    }

    if (GetMatchState() == MatchState::WaitingPostMatch)
    {
        return;
    }
    
    CaptureDuration += DeltaTime;
    if (CaptureDuration >= CaptureUpdateInterval)
    {
        CaptureDuration -= CaptureUpdateInterval;
        UpdateCapturePoint();
    }
    
    ScoreDuration += DeltaTime;
    if (ScoreDuration >= ScoreUpdateInterval)
    {
        ScoreDuration -= ScoreUpdateInterval;
        UpdateTeamScores();
    }
}

void AControlGameState::OnEnterPoint(AShooterCharacter* Character)
{
    if (Character && !CharsContesting.Contains(Character))
    {
        CharsContesting.Add(Character);
    }
}

void AControlGameState::OnLeavePoint(AShooterCharacter* Character)
{
    if (Character)
    {
        CharsContesting.Remove(Character);
    }
}

void AControlGameState::ResetPts()
{
    RedPts = 0;
    BluePts = 0;
}

// handles capturing point... seperate function for calculating scores
void AControlGameState::UpdateCapturePoint()
{
    if (CharsContesting.Num() == 0)
    {
        return;
    }
    
    bool bIsContested = IsContested();

    // make sure teams can't hit 100 without being uncontested... will cap at 99
    if (ControllingTeam == EShooterTeam::Red && RedCount > 0)
    {
        if (CapturePercentage < 99)
        {
            CapturePercentage = FMath::Min(99, CapturePercentage + CaptureRatePerUpdate);
        }
        else if (CapturePercentage == 99 && !bIsContested)
        {
            CapturePercentage = 100;
        }
    }
    else if (ControllingTeam == EShooterTeam::Blue && BlueCount > 0)
    {
        if (CapturePercentage < 99)
        {
            CapturePercentage = FMath::Min(99, CapturePercentage + CaptureRatePerUpdate);
        }
        else if (CapturePercentage == 99 && !bIsContested)
        {
            CapturePercentage = 100;
        }
    }
    else if (!bIsContested)
    {
        // no one has control and it's uncontested
        if (RedCount > 0)
        {
            CapturePercentage = FMath::Min(100, CapturePercentage + CaptureRatePerUpdate);
            
            if (CapturePercentage >= 100)
            {
                ControllingTeam = EShooterTeam::Red;
                CapturePercentage = 100;
            }
        }
        else if (BlueCount > 0)
        {
            CapturePercentage = FMath::Min(100, CapturePercentage + CaptureRatePerUpdate);
            
            if (CapturePercentage >= 100)
            {
                ControllingTeam = EShooterTeam::Blue;
                CapturePercentage = 100;
            }
        }
    }
    // else: contested and no capturing team, do nothing
}

void AControlGameState::UpdateTeamScores()
{
    bool bIsContested = IsContested();
    
    // team has control if capture percentage is 100
    // caps at 99 until uncontested
    if (ControllingTeam == EShooterTeam::Red && CapturePercentage == 100)
    {
        if (RedPercent < 99)
        {
            RedPercent = FMath::Min(99, RedPercent + ScoreRatePerSecond);
        }
        else if (RedPercent == 99 && !bIsContested)
        {
            RedPercent = 100;
            
            UE_LOG(LogTemp, Warning, TEXT("red wins"));
        }
    }
    else if (ControllingTeam == EShooterTeam::Blue && CapturePercentage == 100)
    {
        if (BluePercent < 99)
        {
            BluePercent = FMath::Min(99, BluePercent + ScoreRatePerSecond);
        }
        else if (BluePercent == 99 && !bIsContested)
        {
            BluePercent = 100;
            
            UE_LOG(LogTemp, Warning, TEXT("blue wins"));
        }
    }
}

void AControlGameState::HandleMatchHasEnded()
{
    Super::HandleMatchHasEnded();
}

void AControlGameState::OnRep_CapturePercentage()
{
    // UI can update here when capture percentage changes
    UE_LOG(LogTemp, Log, TEXT("cap Percentage: %d"), CapturePercentage);
}

void AControlGameState::OnRep_ControllingTeam()
{
}

void AControlGameState::OnRep_RedPercent()
{
    UE_LOG(LogTemp, Log, TEXT("red score: %d%%"), RedPercent);
}

void AControlGameState::OnRep_BluePercent()
{
    UE_LOG(LogTemp, Log, TEXT("blue score: %d%%"), BluePercent);
}

void AControlGameState::OnRep_RedPts()
{
    UE_LOG(LogTemp, Log, TEXT("red pts: %d"), RedPts);
}

void AControlGameState::OnRep_BluePts()
{
    UE_LOG(LogTemp, Log, TEXT("blue pts: %d"), BluePts);
}

void AControlGameState::OnRep_RedCount()
{
}

void AControlGameState::OnRep_BlueCount()
{
}

void AControlGameState::MulticastControlAlert_Implementation(const FString& Text, FLinearColor Color, float Duration)
{
    if (AShooterPlayerController* Controller = UShooterBPLibrary::GetShooterController(GetWorld(), 0))
    {
        if (Controller->IsLocalController())
        {
            Controller->OnAlert(Text, Color, Duration);
        }
    }
}

void AControlGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME(AControlGameState, CapturePercentage);
    DOREPLIFETIME(AControlGameState, ControllingTeam);
    DOREPLIFETIME(AControlGameState, RedPercent);
    DOREPLIFETIME(AControlGameState, BluePercent);
    DOREPLIFETIME(AControlGameState, RedPts);
    DOREPLIFETIME(AControlGameState, BluePts);
    DOREPLIFETIME(AControlGameState, RedCount);
    DOREPLIFETIME(AControlGameState, BlueCount);
}

bool AControlGameState::IsContested()
{
    RedCount = 0;
    BlueCount = 0;
    
    for (AShooterCharacter* Char : CharsContesting)
    {
        // dead bodies shouldn't count
        if (!Char || Char->GetCurrentHP() == 0.0f)
        {
            continue;
        }
        
        if (AShooterPlayerState* PS = Char->GetPlayerState<AShooterPlayerState>())
        {
            if (PS->Team == EShooterTeam::Red)
            {
                RedCount++;
            }
            else
            {
                BlueCount++;
            }
        }
    }
    return (RedCount > 0 && BlueCount > 0);
}

void AControlGameState::SetRoundWins(int Red, int Blue)
{
    if (HasAuthority())
    {
        RedPts = Red;
        BluePts = Blue;
    }
}