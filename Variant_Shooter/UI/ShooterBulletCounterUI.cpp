// Copyright Epic Games, Inc. All Rights Reserved.


#include "ShooterBulletCounterUI.h"

#include "ControlGameState.h"
#include "ShooterBPLibrary.h"
#include "ShooterGameState.h"
#include "ShooterPlayerController.h"
#include "ShooterPlayerState.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameMode.h"
#include "Kismet/GameplayStatics.h"

void UShooterBulletCounterUI::OnAlert(const FString& Text, FLinearColor Color, float Duration)
{
	AlertDuration = Duration;
	Alert->SetText(FText::FromString(Text));
	Alert->SetColorAndOpacity(Color);
}

void UShooterBulletCounterUI::TriggerDeath()
{
	DeathDuration = 5.0f;
}

void UShooterBulletCounterUI::AddChatMessage(EShooterTeam Team, const FString& Sender, const FString& Msg)
{
	if (ChatMessageWidgetClass)
	{
		ChatMessageInstance = NewObject<UChatMessageWidget>(this, ChatMessageWidgetClass);
		ChatMessages->AddChildToVerticalBox(ChatMessageInstance);

		// customizing chat message

		// determine channel
		FLinearColor ChannelColor;
		FString ChannelName;
		
		if (Team == EShooterTeam::None)
		{
			ChannelColor = FLinearColor::White;
			ChannelName = "[All]";
		}
		else
		{
			ChannelColor = Team == EShooterTeam::Red ? FLinearColor::Red : FLinearColor::Blue;
			ChannelName = "[Team]";
		}
		ChatMessageInstance->Channel->SetText(FText::FromString(ChannelName));
		ChatMessageInstance->Channel->SetColorAndOpacity(ChannelColor);

		// setting other text blocks
		ChatMessageInstance->Sender->SetText(FText::FromString(Sender));
		ChatMessageInstance->Message->SetText(FText::FromString(Msg));

		ChatScrollBox->ScrollToEnd();
	}
}

void UShooterBulletCounterUI::OpenChat(bool bIsAll)
{
	ChatEntry->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(UShooterBPLibrary::GetShooterController(GetWorld(), 0), ChatTextBox);
	bIsAllChat = bIsAll;
	
	if (!bIsAll)
	{
		ChatTeam->SetText(FText::FromString("[Team]"));
	}
	else
	{
		ChatTeam->SetText(FText::FromString("[All]"));
	}
}

void UShooterBulletCounterUI::ChangeTag(int TagIdx)
{
	TagPreview->SetBrushFromMaterial(TagMaterials[TagIdx - 1]);
}

void UShooterBulletCounterUI::ToggleLoadingCircle(bool bSetVisible)
{
	if (bSetVisible)
	{
		LoadingCircle->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		LoadingCircle->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UShooterBulletCounterUI::UpdatePercentages(EShooterTeam Team, int Percent)
{
	if (Team == EShooterTeam::Red)
	{
		BlueBG->SetOpacity(0.4f);
		RedBG->SetOpacity(1.0f);
		FText NewPercent = FText::Format(INVTEXT("{0}%"), Percent);
		RedPercent->SetText(NewPercent);
	}
	else if (Team == EShooterTeam::Blue)
	{
		RedBG->SetOpacity(0.4f);
		BlueBG->SetOpacity(1.0f);
		FText NewPercent = FText::Format(INVTEXT("{0}%"), Percent);
		BluePercent->SetText(NewPercent);
	}
}

void UShooterBulletCounterUI::UpdateContesting(int RedCt, int BlueCt)
{
	ContMsg->SetVisibility(ESlateVisibility::Hidden);
	
	if (RedCt > 0)
	{
		RedCont->SetText(FText::AsNumber(RedCt));
		NumRedBG->SetVisibility(ESlateVisibility::Visible);
		RedCont->SetVisibility(ESlateVisibility::Visible);
		
		ContMsg->SetVisibility(ESlateVisibility::Visible);
	}
	
	if (BlueCt > 0)
	{
		BlueCont->SetText(FText::AsNumber(BlueCt));
		NumBlueBG->SetVisibility(ESlateVisibility::Visible);
		BlueCont->SetVisibility(ESlateVisibility::Visible);
		
		ContMsg->SetVisibility(ESlateVisibility::Visible);
	}
	
	if (RedCt == 0)
	{
		NumRedBG->SetVisibility(ESlateVisibility::Hidden);
		RedCont->SetVisibility(ESlateVisibility::Hidden);
		ContMsg->SetVisibility(ESlateVisibility::Hidden);
	}
	
	if (BlueCt == 0)
	{
		NumBlueBG->SetVisibility(ESlateVisibility::Hidden);
		BlueCont->SetVisibility(ESlateVisibility::Hidden);
		ContMsg->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UShooterBulletCounterUI::UpdateTeamPts()
{
	if (AControlGameState* GS = UShooterBPLibrary::GetControlGameState(this))
	{
		RedTeamPts->SetText(FText::AsNumber(GS->GetRedPts()));
		BlueTeamPts->SetText(FText::AsNumber(GS->GetBluePts()));
	}
}

void UShooterBulletCounterUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// update red and blue percentages as well as contesting logic
	if (AControlGameState* GameState = UShooterBPLibrary::GetControlGameState(this))
	{
		EShooterTeam InControl = GameState->GetControllingTeam();
		int P = 0;
		if (InControl == EShooterTeam::Red)
		{
			P = GameState->GetRedPercent();
		}
		else if (InControl == EShooterTeam::Blue)
		{
			P = GameState->GetBluePercent();
		}
		UpdatePercentages(InControl, P);

		UpdateTeamPts();
		UpdateContesting(GameState->GetRedOnPt(), GameState->GetBlueOnPt());
	}
	// update red and blue score
	if (AShooterGameState* GameState = UShooterBPLibrary::GetShooterGameState(GetWorld()))
	{
		RedScore->SetText(FText::AsNumber(GameState->RedScore));
		BlueScore->SetText(FText::AsNumber(GameState->BlueScore));

		// If we aren't in progress, show the timer
		// show timer if also on death screen
		if (GameState && GameState->GetMatchState() == MatchState::WaitingToStart)
		{
			// If we can't see the timer, make it visible
			if (Timer->GetVisibility() != ESlateVisibility::HitTestInvisible)
			{
				Timer->SetVisibility(ESlateVisibility::HitTestInvisible);
			}
			FString TimerText = FString::Printf(TEXT("%f"), GameState->WaitingToStartTime);
			Timer->SetText(FText::FromString(TimerText));
		}
		else if (Timer->GetVisibility() != ESlateVisibility::Hidden)
		{
			// Hide this because match is in progress
			Timer->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	// update my score and tag preview
	if (AShooterPlayerController* Controller = UShooterBPLibrary::GetShooterController(GetWorld(), 0))
	{
		if (AShooterPlayerState* PlayerState = Controller->GetPlayerState<AShooterPlayerState>())
		{
			MyScore->SetText(FText::AsNumber(PlayerState->GetScore()));

			TagPreview->SetBrushFromMaterial(TagMaterials[Controller->GetCurrTagIdx() - 1]);
		}
	}

	// displaying alert or death message/countdown
	if (AShooterGameState* GameState = UShooterBPLibrary::GetShooterGameState(GetWorld()))
	{
		if (GameState)
		{
			if (DeathDuration > 0.0f)
			{
				if (Timer->GetVisibility() != ESlateVisibility::HitTestInvisible)
				{
					Timer->SetVisibility(ESlateVisibility::HitTestInvisible);
				}
				FString TimerText = FString::Printf(TEXT("%f"), DeathDuration);
				Timer->SetText(FText::FromString(TimerText));
				
				DeathDuration -= InDeltaTime;

				if (DeathMsg->GetVisibility() == ESlateVisibility::Hidden)
				{
					DeathMsg->SetVisibility(ESlateVisibility::Visible);
				}
			}
			else if (AlertDuration > 0.0f)
			{
				AlertDuration -= InDeltaTime;

				// if it's hidden, make it visible
				if (Alert->GetVisibility() == ESlateVisibility::Hidden)
				{
					Alert->SetVisibility(ESlateVisibility::Visible);
				}
			}
			else // making everything hidden again
			{
				DeathMsg->SetVisibility(ESlateVisibility::Hidden);
				Alert->SetVisibility(ESlateVisibility::Hidden);
			}

			// changing ready check box visibility
			if (GameState->GetMatchState() == MatchState::WaitingToStart)
			{
				ReadyCheckBox->SetVisibility(ESlateVisibility::Visible);
			}
			else
			{
				ReadyCheckBox->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}

void UShooterBulletCounterUI::NativeConstruct()
{
	Super::NativeConstruct();
	
	ChatTextBox->OnTextCommitted.AddDynamic(this, &UShooterBulletCounterUI::HandleTextCommitted);
	ReadyCheckBox->OnCheckStateChanged.AddDynamic(this, &UShooterBulletCounterUI::HandleReadyCheckBox);
}

void UShooterBulletCounterUI::NativeDestruct()
{
	ChatTextBox->OnTextCommitted.RemoveDynamic(this, &UShooterBulletCounterUI::HandleTextCommitted);
	ReadyCheckBox->OnCheckStateChanged.RemoveDynamic(this, &UShooterBulletCounterUI::HandleReadyCheckBox);
	
	Super::NativeDestruct();
}

void UShooterBulletCounterUI::HandleTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	// clear chattextbox
	ChatTextBox->SetText(FText::FromString(""));
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(UShooterBPLibrary::GetShooterController(GetWorld(), 0));

	AShooterPlayerState* PlayerState = GetOwningPlayerState<AShooterPlayerState>();
	if (CommitMethod == ETextCommit::OnEnter)
	{
		if (!bIsAllChat)
		{
			PlayerState->ServerSendMessage(PlayerState->Team, PlayerState->GetPlayerName(), Text.ToString());
		}
		else
		{
			PlayerState->ServerSendMessage(EShooterTeam::None, PlayerState->GetPlayerName(), Text.ToString());
		}
		ChatEntry->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UShooterBulletCounterUI::HandleReadyCheckBox(bool bIsReady)
{
	if (AShooterPlayerState* PS = GetOwningPlayerState<AShooterPlayerState>())
	{
		PS->ServerSetReady(bIsReady);
	}
}
