// Copyright 2025 Lauren Campbell (laurencc@usc.edu)

#pragma once

#include "CoreMinimal.h"
#include "ShooterGameState.h"
#include "ShooterPlayerState.h"
#include "GameFramework/GameState.h"
#include "ControlGameState.generated.h"

class AShooterCharacter;
/**
 * 
 */
UCLASS()
class MULTI_API AControlGameState : public AShooterGameState
{
    GENERATED_BODY()

public:
    AControlGameState();
    
    virtual void Tick(float DeltaTime) override;
    
    void OnEnterPoint(AShooterCharacter* Character);
    void OnLeavePoint(AShooterCharacter* Character);
    
    int GetRedPercent() const { return RedPercent; }
    int GetBluePercent() const { return BluePercent; }
    int GetCapturePercentage() const { return CapturePercentage; }
    EShooterTeam GetControllingTeam() const { return ControllingTeam; }

    void IncreaseRedPts() { RedPts++; }
    void IncreaseBluePts() { BluePts++; }
    void SetRedPts(int Pts) { RedPts = Pts; }
    void SetBluePts(int Pts) { BluePts = Pts; }
    int GetRedPts() const { return RedPts; }
    int GetBluePts() const { return BluePts; }
    void ResetPts();
    
    bool IsContested();
    int GetRedOnPt() const { return RedCount; }
    int GetBlueOnPt() const { return BlueCount; }

    void SetRoundWins(int Red, int Blue);
    
    UFUNCTION(NetMulticast, Reliable)
    void MulticastControlAlert(const FString& Text, FLinearColor Color, float Duration);
    
protected:
    UPROPERTY(Transient)
    TArray<AShooterCharacter*> CharsContesting;
    
    // need to track how close a team is to winning control of point
    UPROPERTY(ReplicatedUsing=OnRep_CapturePercentage)
    int CapturePercentage;
    
    UPROPERTY(ReplicatedUsing=OnRep_ControllingTeam)
    EShooterTeam ControllingTeam;
    
    // these are percentages
    UPROPERTY(ReplicatedUsing=OnRep_RedPercent)
    int RedPercent;
    
    UPROPERTY(ReplicatedUsing=OnRep_BluePercent)
    int BluePercent;

    // these are total match scores
    UPROPERTY(ReplicatedUsing=OnRep_RedPts)
    int RedPts;
    
    UPROPERTY(ReplicatedUsing=OnRep_BluePts)
    int BluePts;

    // number of ppl on point
    UPROPERTY(ReplicatedUsing=OnRep_RedCount)
    int RedCount = 0;
    
    UPROPERTY(ReplicatedUsing=OnRep_BlueCount)
    int BlueCount = 0;
    
    // setting intervals for capture and scoring rates
    float CaptureDuration = 0.0f;
    
    float ScoreDuration = 0.0f;
    
    UPROPERTY(EditDefaultsOnly, Category = "Capture")
    float CaptureUpdateInterval = 0.1f;
    
    UPROPERTY(EditDefaultsOnly, Category = "Capture")
    float ScoreUpdateInterval = 1.0f;
    
    // capture rate: 100% / 5 seconds / 10 updates per second = 2:1 update
    UPROPERTY(EditDefaultsOnly, Category = "Capture")
    int CaptureRatePerUpdate = 2;
    
    UPROPERTY(EditDefaultsOnly, Category = "Capture")
    int32 ScoreRatePerSecond = 3; // remember should be 1, will likely change for demonstration
    
    UFUNCTION()
    void OnRep_CapturePercentage();
    
    UFUNCTION()
    void OnRep_ControllingTeam();
    
    UFUNCTION()
    void OnRep_RedPercent();
    
    UFUNCTION()
    void OnRep_BluePercent();
    
    UFUNCTION()
    void OnRep_RedPts();
    
    UFUNCTION()
    void OnRep_BluePts();
    
    UFUNCTION()
    void OnRep_RedCount();
    
    UFUNCTION()
    void OnRep_BlueCount();
    
    void UpdateCapturePoint();
    void UpdateTeamScores();

    virtual void HandleMatchHasEnded() override;
    
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};