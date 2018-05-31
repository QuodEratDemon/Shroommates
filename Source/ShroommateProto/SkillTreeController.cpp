// Fill out your copyright notice in the Description page of Project Settings.

#include "SkillTreeController.h"
#include "Interactable.h"
#include "ShroommateProtoGameMode.h"
#include "ShroommateProtoCharacter.h"
#include "Blueprint/UserWidget.h"

ASkillTreeController::ASkillTreeController()
{
	insidebuyupgradezone = false;
	haskey = false;
	hasboombox = false;
	hascd1 = false;
	hascd2 = false;
	hascd3 = false;
	hasbattery = false;
	haskitchenitem = false;
	hasspinach = false;
	correctmusic = false;
	cdplaying = false;
}

void ASkillTreeController::BeginPlay()
{
	Super::BeginPlay();
}

void ASkillTreeController::setzone() {
	insidebuyupgradezone = true;
}
void ASkillTreeController::freezone() {
	insidebuyupgradezone = false;
}

void ASkillTreeController::accessstore() {
	if (insidebuyupgradezone) {
		if (wStore != nullptr) {
			//creates widget
			MyStore = CreateWidget<UUserWidget>(this, wStore);
			//check if MyStore succesfully created widget
			if (MyStore != nullptr) {
				//add the widget to the screen
				MyStore->AddToViewport();
			}
			//make mouse cursor show up
			bShowMouseCursor = true;
			//make player not move while upgrading
			ASkillTreeController::SetInputMode(FInputModeUIOnly());
		}
	}
}

void ASkillTreeController::accessskill() {
		if (wSkillTree != nullptr) {
			//creates widget
			MySkillTree = CreateWidget<UUserWidget>(this, wSkillTree);
			//check if MySkillTree succesfully created widget
			if (MySkillTree != nullptr) {
				//add the widget to the screen
				MySkillTree->AddToViewport();
			}
			//make mouse cursor show up
			bShowMouseCursor = true;
			//make player not move while upgrading
			ASkillTreeController::SetInputMode(FInputModeUIOnly());
		}
}

void ASkillTreeController::SetupInputComponent() {
	Super::SetupInputComponent();

	//InputComponent->BindAction("Interact", IE_Pressed, this, &ASkillTreeController::Interact);
}

void ASkillTreeController::CraftItem(FInventoryItem ItemA, FInventoryItem ItemB, ASkillTreeController* Controller) {
	
	for (auto Craft : ItemB.CraftCombinations) {
		if (Craft.ComponentID == ItemA.ItemID) {
			if (Craft.bDestroyItemA) {
				Inventory.RemoveSingle(ItemA);
			}
			if (Craft.bDestroyItemB) {
				Inventory.RemoveSingle(ItemB);
			}
			AddItemToInventoryByID(Craft.ProductID);
			ReloadInventory();
		}
	}

}

void ASkillTreeController::Interact() {

	if (CurrentInteractable) {
		CurrentInteractable->Interact(this);
	}
}

void ASkillTreeController::AddItemToInventoryByID(FName ID) {

	AShroommateProtoGameMode* GameMode = Cast<AShroommateProtoGameMode>(GetWorld()->GetAuthGameMode());
	UDataTable* ItemTable = GameMode->GetItemDB();

	FInventoryItem* ItemToAdd = ItemTable->FindRow<FInventoryItem>(ID, "");

	if (ItemToAdd) {
		Inventory.Add(*ItemToAdd);
	}
}