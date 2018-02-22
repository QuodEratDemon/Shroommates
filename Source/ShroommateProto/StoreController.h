// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "StoreController.generated.h"

/**
 * 
 */
UCLASS()
class SHROOMMATEPROTO_API AStoreController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AStoreController();

	//Make wSkillTree subclass of UserWidget
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		TSubclassOf<class UUserWidget> wStore;

	UUserWidget* MyStore;

	//override BeginPlay() function in UserWidget
	virtual void BeginPlay() override;
	
	
};
