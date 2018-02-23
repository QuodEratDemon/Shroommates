// Fill out your copyright notice in the Description page of Project Settings.

#include "SkillTreeController.h"
#include "Blueprint/UserWidget.h"

ASkillTreeController::ASkillTreeController()
{
	insidebuyupgradezone = false;
}

void ASkillTreeController::BeginPlay()
{
	Super::BeginPlay();
	/*
	//check if wSkillTree is not NULL, it will crash if it is NULL
	if (insidebuyupgradezone) {
		if (wSkillTree != nullptr) {
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Skilltree"));
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
	
	//store
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
	}*/
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