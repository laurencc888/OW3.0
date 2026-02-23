// Copyright 2025 Lauren Campbell (laurencc@usc.edu)


#include "ShooterBPLibrary.h"

#include "ControlGameMode.h"
#include "ControlGameState.h"
#include "ShooterCharacter.h"
#include "ShooterGameMode.h"
#include "ShooterGameState.h"
#include "ShooterPlayerController.h"
#include "Kismet/GameplayStatics.h"

class AShooterCharacter* UShooterBPLibrary::GetShooterCharacter(const UObject* WorldContextObject, int32 PlayerIndex)
{
	return Cast<AShooterCharacter>(UGameplayStatics::GetPlayerCharacter(WorldContextObject, PlayerIndex));
}

class AShooterPlayerController* UShooterBPLibrary::GetShooterController(const UObject* WorldContextObject,
	int32 PlayerIndex)
{
	return Cast<AShooterPlayerController>(UGameplayStatics::GetPlayerController(WorldContextObject, PlayerIndex));
}

class AShooterGameMode* UShooterBPLibrary::GetShooterGameMode(const UObject* WorldContextObject)
{
	return Cast<AShooterGameMode>(UGameplayStatics::GetGameMode(WorldContextObject));
}

class AShooterGameState* UShooterBPLibrary::GetShooterGameState(const UObject* WorldContextObject)
{
	return Cast<AShooterGameState>(UGameplayStatics::GetGameState(WorldContextObject));
}

class AControlGameMode* UShooterBPLibrary::GetControlGameMode(const UObject* WorldContextObject)
{
	return Cast<AControlGameMode>(UGameplayStatics::GetGameMode(WorldContextObject));
}

class AControlGameState* UShooterBPLibrary::GetControlGameState(const UObject* WorldContextObject)
{
	return Cast<AControlGameState>(UGameplayStatics::GetGameState(WorldContextObject));
}
