// Fill out your copyright notice in the Description page of Project Settings.

#include "darkness.h"
#include "ShroommateProtoCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "Qualities3.h"
#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White,text)


// Sets default values
Adarkness::Adarkness()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//set hitbox
	Hitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("hitbox"));
	//sethitboxsize
	Hitbox->SetWorldScale3D(FVector(1.f, 1.f, 0.2f));
	Hitbox->bGenerateOverlapEvents = true;
	Hitbox->OnComponentBeginOverlap.AddDynamic(this, &Adarkness::onPlayerEnterShadow);
	Hitbox->OnComponentEndOverlap.AddDynamic(this, &Adarkness::onPlayerExitShadow);
	Hitbox->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void Adarkness::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void Adarkness::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (inShade && objectInShade->GetName() == "Character") {
		AShroommateProtoCharacter* tempChar = Cast<AShroommateProtoCharacter>(objectInShade);
		UPostProcessComponent* tempPPC = tempChar->FindComponentByClass<UPostProcessComponent>();
		tempPPC->Settings.AutoExposureMinBrightness = 10.f;


		timeInShade += DeltaTime;
		if (timeInShade > 0.5f) {
			FVector NewScale = objectInShade->GetActorScale();
			objectInShade->SetActorRelativeScale3D(NewScale + FVector(0.001f, 0.001f, 0.001f));
			tempChar = Cast<AShroommateProtoCharacter>(objectInShade);
			UQualities3* tempq = tempChar->FindComponentByClass<UQualities3>();
			//UPostProcessComponent* tempPPC = tempChar->FindComponentByClass<UPostProcessComponent>()
			//for testing purposes
			tempq->addToLight(change);
			//print("Light: "+FString::SanitizeFloat(tempq->getLight()));
			timeInShade = 0;
		}
		//print(FString::SanitizeFloat(DeltaTime));

	}
	
	else if (justOut && !inShade && objectOutShade->GetName() == "Character") {

		AShroommateProtoCharacter* tempChar = Cast<AShroommateProtoCharacter>(objectOutShade);
		UPostProcessComponent* tempPPC = tempChar->FindComponentByClass<UPostProcessComponent>();
		tempPPC->Settings.AutoExposureMinBrightness =0.03f;
	}
	
	

}

void Adarkness::onPlayerEnterShadow(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	objectInShade = OtherActor;
	inShade = true;

}

void Adarkness::onPlayerExitShadow(UPrimitiveComponent* OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	objectOutShade = OtherActor;
	inShade = false;
	timeInShade = 0;
	justOut = true;
}
