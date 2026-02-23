// Copyright 2025 Lauren Campbell (laurencc@usc.edu)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "ChatMessageWidget.generated.h"

/**
 * 
 */
UCLASS()
class MULTI_API UChatMessageWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Channel;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Sender;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Message;
};
