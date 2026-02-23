// Copyright 2025 Lauren Campbell (laurencc@usc.edu)


#include "MultiOnlineSubsystem.h"

#include "Online/Lobbies.h"
#include "Online/OnlineAsyncOp.h"
#include "Online/OnlineResult.h"
#include "Online/OnlineSessionNames.h"

void UMultiOnlineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	OnlineServices = GetServices();
	if (OnlineServices)
	{
		AuthInterface = OnlineServices->GetAuthInterface();
		LobbiesInterface = OnlineServices->GetLobbiesInterface();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Could not retrieve online services."));
	}
	
}

bool UMultiOnlineSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
#if WITH_EDITOR
	return false;
#else
	return true;
#endif
}

void UMultiOnlineSubsystem::LoginHelper(FName CredentialsType)
{
	FAuthLogin::Params Params;
	if (ULocalPlayer* Player = GetGameInstance()->GetLocalPlayerByIndex(0))
	{
		Params.PlatformUserId = Player->GetPlatformUserId();
	}
	Params.CredentialsType = CredentialsType;

	AuthInterface->Login(MoveTemp(Params)).OnComplete([this, CredentialsType](const TOnlineResult<FAuthLogin>& Result)
	{
		if (Result.IsOk())
		{
			AccountInfo = Result.GetOkValue().AccountInfo.Get();
		}
		else if (CredentialsType == LoginCredentialsType::PersistentAuth)
		{
			LoginHelper(LoginCredentialsType::AccountPortal);
		}
	});
}

void UMultiOnlineSubsystem::Login()
{
	LoginHelper(LoginCredentialsType::PersistentAuth);
}

bool UMultiOnlineSubsystem::IsLoggedIn() const
{
	return AccountInfo.LoginStatus == ELoginStatus::LoggedIn;
}

void UMultiOnlineSubsystem::HostSession()
{
	if (!LobbiesInterface || !IsLoggedIn())
	{
		return;
	}

	FCreateLobby::Params Params = {AccountInfo.AccountId, TEXT("MyGame"), FSchemaId(TEXT("GameLobby")), true, 4, ELobbyJoinPolicy::PublicAdvertised};

	LobbiesInterface->CreateLobby(MoveTemp(Params)).OnComplete([this](const TOnlineResult<FCreateLobby>& Result)
	{
		if (Result.IsOk())
		{
			// load level in listen server mode
			GetWorld()->ServerTravel("Lvl_Shooter?listen");
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("failed to create lobby"));
		}
	});
}

// taken from original host session function
void UMultiOnlineSubsystem::HostControlSession()
{
	if (!LobbiesInterface || !IsLoggedIn())
	{
		return;
	}

	FCreateLobby::Params Params = {AccountInfo.AccountId, TEXT("MyGame"), FSchemaId(TEXT("GameLobby")), true, 4, ELobbyJoinPolicy::PublicAdvertised};

	LobbiesInterface->CreateLobby(MoveTemp(Params)).OnComplete([this](const TOnlineResult<FCreateLobby>& Result)
	{
		if (Result.IsOk())
		{
			// load level in listen server mode
			GetWorld()->ServerTravel("Lvl_Control?listen");
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("failed to create lobby"));
		}
	});
}

void UMultiOnlineSubsystem::FindAndJoinSession()
{
	if (!LobbiesInterface || !IsLoggedIn())
	{
		return;
	}

	FFindLobbies::Params FindParams = {AccountInfo.AccountId};
	LobbiesInterface->FindLobbies(MoveTemp(FindParams)).OnComplete([this](const TOnlineResult<FFindLobbies>& FindResult)
	{
		if (FindResult.IsOk())
		{
			const FFindLobbies::Result& FindResults = FindResult.GetOkValue();
			for (auto Lobby : FindResults.Lobbies)
			{
				if (Lobby->OwnerAccountId.IsValid() && Lobby->Members.Num() > 0)
				{
					// Well there's someone here and the lobby owner is too, so let's join...
					FJoinLobby::Params JoinParams = {AccountInfo.AccountId, TEXT("MyGame"), Lobby->LobbyId, true};
					LobbiesInterface->JoinLobby(MoveTemp(JoinParams)).OnComplete([this](const TOnlineResult<FJoinLobby>& JoinResult)
					{
						if (JoinResult.IsOk())
						{
							// get URL for lobby to join
							auto Lobby = JoinResult.GetOkValue().Lobby;
							TOnlineResult<FGetResolvedConnectString> ConnectResult = OnlineServices->GetResolvedConnectString({AccountInfo.AccountId, Lobby->LobbyId});
							if (ConnectResult.IsOk())
							{
								APlayerController* PC = GetGameInstance()->GetFirstLocalPlayerController();
								PC->ClientTravel(ConnectResult.GetOkValue().ResolvedConnectString, TRAVEL_Absolute);
							}
						}
					});
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("couldn't find lobbies"));
		}
	});
}
