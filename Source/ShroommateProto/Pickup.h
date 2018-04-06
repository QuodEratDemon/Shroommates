// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "Pickup.generated.h"

/**
 * 
 */
UCLASS()
class SHROOMMATEPROTO_API APickup : public AInteractable
{
	GENERATED_BODY()
	
public:

	APickup();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Pickup")
		void OnUsed();

protected:

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* PickupMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName ItemID;
	
	
};
