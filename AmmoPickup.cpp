// Copyright 2025 Lauren Campbell (laurencc@usc.edu)


#include "AmmoPickup.h"

#include "ShooterCharacter.h"
#include "ShooterWeapon.h"

// Sets default values
AAmmoPickup::AAmmoPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetSphereRadius(25.0f);
	RootComponent = SphereComponent;

	RenderComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("RenderComponent"));
	RenderComponent->SetText(FText::FromString("Ammo"));
	RenderComponent->SetVerticalAlignment(EVRTA_TextCenter);
	RenderComponent->SetHorizontalAlignment(EHTA_Center);
	RenderComponent->SetTextRenderColor(FColor::Green);
	RenderComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AAmmoPickup::BeginPlay()
{
	Super::BeginPlay();
}

void AAmmoPickup::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AAmmoPickup::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (GetLocalRole() == ROLE_Authority)
	{
		if (AShooterCharacter* Character = Cast<AShooterCharacter>(OtherActor))
		{
			if (AShooterWeapon* Weapon = Character->GetCurrentWeapon())
			{
				if (!Weapon->IsFullAmmo())
				{
					Weapon->AddAmmo(AmountAmmo);
					Destroy();
				}
			}
		}
	}
}

// Called every frame
void AAmmoPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

