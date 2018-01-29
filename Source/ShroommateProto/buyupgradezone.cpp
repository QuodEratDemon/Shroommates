// Fill out your copyright notice in the Description page of Project Settings.

#include "buyupgradezone.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "ShroommateProtoCharacter.h"
#include "SkillTreeController.h"

// Sets default values
Abuyupgradezone::Abuyupgradezone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Hitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("hitbox"));
	//sethitboxsize
	Hitbox->SetWorldScale3D(FVector(1.f, 1.f, 0.2f));
	Hitbox->bGenerateOverlapEvents = true;
	Hitbox->OnComponentBeginOverlap.AddDynamic(this, &Abuyupgradezone::onPlayerEnterZone);
	Hitbox->OnComponentEndOverlap.AddDynamic(this, &Abuyupgradezone::onPlayerExitZone);
	Hitbox->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void Abuyupgradezone::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void Abuyupgradezone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (insidezone && objectinzone->GetName() == "Character") {
		AShroommateProtoCharacter* tempChar = Cast<AShroommateProtoCharacter>(objectinzone);
		if (tempChar->a != nullptr) {
			class ASkillTreeController* tempq = tempChar->a;
			tempq->setzone();
		}
	}
	
	if(insidezone == false && objectinzone != nullptr){
		AShroommateProtoCharacter* tempChar = Cast<AShroommateProtoCharacter>(objectinzone);
		if (tempChar->a != nullptr) {
			class ASkillTreeController* tempq = tempChar->a;
			tempq->freezone();
		}
	}

}

void Abuyupgradezone::onPlayerEnterZone(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	objectinzone = OtherActor;
	insidezone = true;

}

void Abuyupgradezone::onPlayerExitZone(UPrimitiveComponent* OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	insidezone = false;
}