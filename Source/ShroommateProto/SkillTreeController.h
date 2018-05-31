// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShroommateProtoCharacter.h"
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

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	class AInteractable* CurrentInteractable;

	UFUNCTION()
	void Interact();

	UFUNCTION(BlueprintImplementableEvent)
		void ReloadInventory();

	UFUNCTION(BlueprintCallable, Category = "Utils")
	void AddItemToInventoryByID(FName ID);

	UFUNCTION(BlueprintCallable, Category = "Utils")
	void CraftItem(FInventoryItem ItemA, FInventoryItem ItemB, ASkillTreeController* Controller);

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TArray<FInventoryItem> Inventory;

	//Make wSkillTree subclass of UserWidget
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		TSubclassOf<class UUserWidget> wSkillTree;

	UUserWidget* MySkillTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		TSubclassOf<class UUserWidget> wStore;

	UUserWidget* MyStore;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool insidebuyupgradezone;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool haskey;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool hasboombox;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool hascd1;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool hascd2;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool hascd3;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool hasbattery;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool haskitchenitem;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool hasspinach;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool correctmusic;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool cdplaying;

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

protected:

	virtual void SetupInputComponent() override;
};
