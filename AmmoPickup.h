// Copyright 2025 Lauren Campbell (laurencc@usc.edu)

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/Actor.h"
#include "AmmoPickup.generated.h"

UCLASS()
class MULTI_API AAmmoPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAmmoPickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere)
	UTextRenderComponent* RenderComponent;

private:
	UPROPERTY(EditAnywhere)
	int AmountAmmo = 5;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
