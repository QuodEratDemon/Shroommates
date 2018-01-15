// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "GameFramework/Actor.h"
#include "darkness.generated.h"

UCLASS()
class SHROOMMATEPROTO_API Adarkness : public AActor
{
	GENERATED_BODY()
		UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* DarknessMesh;

	//set pickup hitbox
	UPROPERTY(EditAnywhere)
		class UShapeComponent* Hitbox;
	UPROPERTY(EditAnywhere)
		float change;
public:
	// Sets default values for this actor's properties
	Adarkness();
	bool inShade = false;
	float timeInShade = 0.f;
	AActor* objectInShade;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION()
		void onPlayerEnterShadow(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void onPlayerExitShadow(UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


};