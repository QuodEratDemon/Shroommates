// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "buyupgradezone.generated.h"

UCLASS()
class SHROOMMATEPROTO_API Abuyupgradezone : public AActor
{
	GENERATED_BODY()
		UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* buyupgradezoneMesh;

	UPROPERTY(EditAnywhere)
		class UShapeComponent* Hitbox;
	UPROPERTY(EditAnywhere)
		float change;
	
public:	
	// Sets default values for this actor's properties
	Abuyupgradezone();
	bool insidezone = false;
	AActor* objectinzone;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void onPlayerEnterZone(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void onPlayerExitZone(UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
};
