// Fill out your copyright notice in the Description page of Project Settings.

#include "ClamborPoint.h"
#include "ShroommateProtoCharacter.h"

// Sets default values
AClamborPoint::AClamborPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//AG 10/22/17: Hitbox code from Growtest --------------------------------
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootMesh"));

	//set hitbox
	Hitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("hitbox"));
	//sethitboxsize
	Hitbox->SetWorldScale3D(FVector(1.f, 1.f, 1.f));
	Hitbox->bGenerateOverlapEvents = true;
	Hitbox->OnComponentBeginOverlap.AddDynamic(this, &AClamborPoint::onPlayerEnter);
	Hitbox->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AClamborPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AClamborPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void AClamborPoint::onPlayerEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (OtherActor->GetName() == "Character") {
		AShroommateProtoCharacter* tempChar = Cast<AShroommateProtoCharacter>(OtherActor);
		tempChar->canWall = true;
	}
}

void AClamborPoint::onPlayerExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (OtherActor->GetName() == "Character") {
		AShroommateProtoCharacter* tempChar = Cast<AShroommateProtoCharacter>(OtherActor);
		tempChar->canWall = false;
	}

}

