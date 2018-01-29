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
	//check if wSkillTree is not NULL, it will crash if it is NULL
	if (insidebuyupgradezone) {
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
}

void ASkillTreeController::setzone() {
	insidebuyupgradezone = true;
}
void ASkillTreeController::freezone() {
	insidebuyupgradezone = false;
}

