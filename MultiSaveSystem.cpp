// Copyright 2025 Lauren Campbell (laurencc@usc.edu)


#include "MultiSaveSystem.h"

#include "ShooterBPLibrary.h"
#include "ShooterGameMode.h"
#include "ShooterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "TagActor.h"

void UMultiSaveSystem::SaveGame(bool bIsAsync)
{
	if (!bIsAsync)
	{
		UGameplayStatics::SaveGameToSlot(GroundTruth, "SaveGame",0);
	}
	else
	{
		UGameplayStatics::AsyncSaveGameToSlot(GroundTruth, "SaveGame", 0);

		if (AShooterGameMode* GM = UShooterBPLibrary::GetShooterGameMode(GetWorld()))
		{
			GM->TriggerCircleCooldown();
		}
	}
}

void UMultiSaveSystem::LoadGame()
{
	if (USaveGame* SavedGame = UGameplayStatics::LoadGameFromSlot("SaveGame", 0))
	{
		GroundTruth = Cast<UMultiSaveGame>(SavedGame);
		for (FTagActorSaveData ActorData : GroundTruth->TagActors)
		{
			// taken from given code in controller
			// Setup spawn parameters to always spawn the tag actor, even if colliding
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			// Spawn at the saved location with specified rotation
			FTransform Trans = ActorData.Transform;
			//ATagActor* SpawnedTag = GetWorld()->SpawnActor<ATagActor>(SpawnParameters);
			ATagActor* SpawnedTag = GetWorld()->SpawnActor<ATagActor>(ActorData.ClassType, Trans.GetLocation(), FRotator::ZeroRotator, SpawnParameters);
			SpawnedTag->SetActorRotation(Trans.GetRotation());
			SpawnedTag->SetActorScale3D(Trans.GetScale3D());
		}
		
	}
	else
	{
		GroundTruth = NewObject<UMultiSaveGame>();
	}
}

void UMultiSaveSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	GroundTruth = NewObject<UMultiSaveGame>();
}

void UMultiSaveSystem::NewSave()
{
	GroundTruth = NewObject<UMultiSaveGame>();
	UGameplayStatics::SaveGameToSlot(GroundTruth, "SaveGame", 0);
}

bool UMultiSaveSystem::HasSavedGame()
{
	return UGameplayStatics::DoesSaveGameExist("SaveGame", 0);
}

void UMultiSaveSystem::NewControlSave()
{
	GroundTruth = NewObject<UMultiSaveGame>();
	UGameplayStatics::SaveGameToSlot(GroundTruth, "SaveControlGame", 0);
}
