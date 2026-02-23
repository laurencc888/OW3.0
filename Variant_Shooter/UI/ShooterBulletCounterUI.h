// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ChatMessageWidget.h"
#include "ShooterPlayerState.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/CheckBox.h"
#include "Components/CircularThrobber.h"
#include "Components/EditableTextBox.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "ShooterBulletCounterUI.generated.h"

/**
 *  Simple bullet counter UI widget for a first person shooter game
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEditableTextBoxCommittedEvent, const FText&, Text, ETextCommit::Type, CommitMethod);

UCLASS(abstract)
class MULTI_API UShooterBulletCounterUI : public UUserWidget
{
	GENERATED_BODY()
	
public:

	/** Allows Blueprint to update sub-widgets with the new bullet count */
	UFUNCTION(BlueprintImplementableEvent, Category="Shooter", meta=(DisplayName = "UpdateBulletCounter"))
	void BP_UpdateBulletCounter(int32 MagazineSize, int32 BulletCount);

	/** Allows Blueprint to update sub-widgets with the new life total and play a damage effect on the HUD */
	UFUNCTION(BlueprintImplementableEvent, Category="Shooter", meta=(DisplayName = "Damaged"))
	void BP_Damaged(float LifePercent);

	void OnAlert(const FString& Text, FLinearColor Color, float Duration);

	void TriggerDeath();

	void AddChatMessage(EShooterTeam Team, const FString& Sender, const FString& Msg);

	void OpenChat(bool bIsAll);

	void ChangeTag(int TagIdx);

	void ToggleLoadingCircle(bool bSetVisible);

	void UpdatePercentages(EShooterTeam Team, int Percent);

	void UpdateContesting(int RedCt, int BlueCt);

	void UpdateTeamPts();

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> RedScore;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> BlueScore;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> MyScore;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Timer;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Alert;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> DeathMsg;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UVerticalBox> ChatBox;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScrollBox> ChatScrollBox;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UVerticalBox> ChatMessages;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UHorizontalBox> ChatEntry;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> ChatTeam;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UEditableTextBox> ChatTextBox;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCheckBox> ReadyCheckBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> TagPreview;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCircularThrobber> LoadingCircle;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> RedBG;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> BlueBG;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> RedPercent;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> BluePercent;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> RedTeamPts;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> BlueTeamPts;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> RedPtBG;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> BluePtBG;

	
	// contesting ui stuff
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> RedCont;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> BlueCont;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UBorder> NumRedBG;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UBorder> NumBlueBG;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> ContMsg;
	
	

	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<UMaterialInterface>> TagMaterials;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UChatMessageWidget> ChatMessageWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UChatMessageWidget> ChatMessageInstance;

	UFUNCTION()
	void HandleTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void HandleReadyCheckBox(bool bIsReady);

private:
	float AlertDuration = 0.0f;
	float DeathDuration = 0.0f;

	bool bIsAllChat = false;

	EShooterTeam CurrChatTeam = EShooterTeam::None;
};
