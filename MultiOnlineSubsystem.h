// Copyright 2025 Lauren Campbell (laurencc@usc.edu)

#pragma once

#include "CoreMinimal.h"
#include "Online/Auth.h"
#include "Online/OnlineServices.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MultiOnlineSubsystem.generated.h"

using namespace UE::Online;

/**
 * 
 */
UCLASS()
class MULTI_API UMultiOnlineSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void Login();

	UFUNCTION(BlueprintPure)
	bool IsLoggedIn() const;

	UFUNCTION(BlueprintCallable)
	void HostSession();

	UFUNCTION(BlueprintCallable)
	void HostControlSession();

	UFUNCTION(BlueprintCallable)
	void FindAndJoinSession();

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	// Various online interface pointers
	IOnlineServicesPtr OnlineServices;
	IAuthPtr AuthInterface;
	ILobbiesPtr LobbiesInterface;

	// Stores info of the account that's logged in
	FAccountInfo AccountInfo;

	// Helper function for login functionality
	void LoginHelper(FName CredentialsType);
};
