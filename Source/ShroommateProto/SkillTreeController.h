// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SkillTreeController.generated.h"

/**
 * 
 */
UCLASS()
class SHROOMMATEPROTO_API ASkillTreeController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ASkillTreeController();

	//Make wSkillTree subclass of UserWidget
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		TSubclassOf<class UUserWidget> wSkillTree;

	UUserWidget* MySkillTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		TSubclassOf<class UUserWidget> wStore;

	UUserWidget* MyStore;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool insidebuyupgradezone;

	//override BeginPlay() function in UserWidget
	virtual void BeginPlay() override;
	
	UFUNCTION()
		void setzone();
	UFUNCTION()
		void freezone();

	UFUNCTION()
		void accessstore();
	UFUNCTION()
		void accessskill();
};
