// Fill out your copyright notice in the Description page of Project Settings.

#include "climb.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "ShroommateProtoCharacter.h"

// Sets default values
Aclimb::Aclimb()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Hitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("hitbox"));
	//sethitboxsize
	Hitbox->SetWorldScale3D(FVector(1.f, 1.f, 0.2f));
	Hitbox->bGenerateOverlapEvents = true;
	Hitbox->OnComponentBeginOverlap.AddDynamic(this, &Aclimb::onPlayerEnterZone);
	Hitbox->OnComponentEndOverlap.AddDynamic(this, &Aclimb::onPlayerExitZone);
	Hitbox->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void Aclimb::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void Aclimb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (insidezone && objectinzone->GetName() == "Character") {
		AShroommateProtoCharacter* tempChar = Cast<AShroommateProtoCharacter>(objectinzone);
		tempChar->SetClimb(true);
	}

	if (insidezone == false && objectinzone != nullptr) {
		AShroommateProtoCharacter* tempChar = Cast<AShroommateProtoCharacter>(objectinzone);
		tempChar->SetClimb(false);
	}
}

void Aclimb::onPlayerEnterZone(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	objectinzone = OtherActor;
	insidezone = true;

}

void Aclimb::onPlayerExitZone(UPrimitiveComponent* OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	insidezone = false;
}