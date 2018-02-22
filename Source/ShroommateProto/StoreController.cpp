// Fill out your copyright notice in the Description page of Project Settings.

#include "StoreController.h"
#include "Blueprint/UserWidget.h"

AStoreController::AStoreController()
{
	
}

void AStoreController::BeginPlay()
{
	Super::BeginPlay();

		if (wStore != nullptr) {
			//creates widget
			MyStore = CreateWidget<UUserWidget>(this, wStore);
			//check if MySkillTree succesfully created widget
			if (MyStore != nullptr) {
				//add the widget to the screen
				MyStore->AddToViewport();
			}
			//make mouse cursor show up
			bShowMouseCursor = true;
			//make player not move while upgrading
			AStoreController::SetInputMode(FInputModeUIOnly());
		}
}



