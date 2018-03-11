// Fill out your copyright notice in the Description page of Project Settings.

#include "Qualities3.h"
#include "ShroommateProtoCharacter.h"
#include <math.h> 
#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White,text)
/*
#define print2(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Purple,text)
#define print3(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green,text)
#define print4(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Red,text)*/




// Sets default values for this component's properties
UQualities3::UQualities3()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;


	light = 60.f; //Having them all start at 0.5f for now
	hunger = 60.f;
	humidity = 0.61f;

}


// Called when the game starts
void UQualities3::BeginPlay()
{
	Super::BeginPlay();
	player = GetOwner();
	// ...

}


// Called every frame
void UQualities3::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	//Player Scaling
	timeTick += DeltaTime;
	FVector NewScale = player->GetActorScale();
	float growthAmount = growthRate();;
	if (timeTick >= 3.f) {
		//if (NewScale.X + growthAmount > .1) player->SetActorRelativeScale3D(NewScale + FVector(growthAmount, growthAmount, growthAmount));
		
		//Decay qualities
		addToLight(-decayRate);
		addToHunger(-decayRate);
		//addToHumidity(-decayRate);

		curSize = NewScale.X;
		if (curSize > largestSize) largestSize = curSize;

		timeTick = 0.f;
	}
	sizeState = updateSize();

	if (changeSize) {
		AShroommateProtoCharacter* tempChar = Cast<AShroommateProtoCharacter>(player);
		USpringArmComponent* tempCam = tempChar->GetCameraBoom();

		switch (sizeState)
		{
		case 0: {
			player->SetActorRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
			tempCam->TargetArmLength = 2000;
			break;
		}
		case 1: {
			player->SetActorRelativeScale3D(FVector(1.f, 1.f, 1.f));
			tempCam->TargetArmLength = 2500;
			break;
		}
		case 2: {
			player->SetActorRelativeScale3D(FVector(1.5f, 1.5f, 1.5f));
			tempCam->TargetArmLength = 3000;
			break;
		}

				
		}

		changeSize = false;
	}

	//Update size scores


	DisplayGrowthProgress = (light + hunger) / 2;
	
}

// My functions -------------------------------------------------------

//Adds to them (can use negative numbers for subtracting)
float UQualities3::addToLight(float change) {
	light = FMath::Clamp(light += change, 0.f, 1000.f);
	return light;
}

float UQualities3::addToHunger(float change) {
	hunger = FMath::Clamp(hunger += change, 0.f, 1000.f);
	return hunger;
}

float UQualities3::addToHumidity(float change) {
	humidity = FMath::Clamp(humidity += change, 0.f, 1000.f);
	return humidity;
}

//setters
float UQualities3::setLight(float newLight) {
	if (newLight >= 1) {
		light = 1;
	}
	else if (newLight <= 0) {
		light = 0;
	}
	else {
		light = newLight;
	}
	return light;
}

float UQualities3::setHunger(float newTemp) {
	if (newTemp >= 1) {
		hunger = 1;
	}
	else if (newTemp <= 0) {
		hunger = 0;
	}
	else {
		hunger = newTemp;
	}
	return hunger;
}

float UQualities3::setHumidity(float newHumidity) {
	if (newHumidity >= 1) {
		humidity = 1;
	}
	else if (newHumidity <= 0) {
		humidity = 0;
	}
	else {
		humidity = newHumidity;
	}
	return humidity;
}

//getters
float UQualities3::getLight() {
	return light;
}

float UQualities3::getHunger() {
	return hunger;
}

float UQualities3::getHumidity() {
	return humidity;
}

//AG 10/14/17: Helper function for growthRate()
int UQualities3::qualityState(float q) {
	int Qstate = 0;
	if (q >= 0.5 + threshold) Qstate = 1;
	else if (q < 0.5 - threshold) Qstate = -1;
	return Qstate;
}

//AG 10/16/17: Helper function for growthRate()
int UQualities3::qualityHunger(float q) {
	int Qstate = 0;
	if (q < 0.5 - threshold) Qstate = 1;
	else if (q >= 0.5 + threshold) Qstate = -1;
	return Qstate;
}


float UQualities3::growthRate() {


	//Check qualities' relation to thresholds
	int humState = qualityState(humidity);
	int hunState = qualityState(hunger);//Hunger(hunger);
	int ligState = qualityState(light);
   


	int state = humState + hunState + ligState;
	if (state == -3) {
		return -.0001; //-y=(.02)^1.1
	}
	else if (state == -2) {
		return -.00004; //-y=(.02)
	}
	else if (state == -1) {
		return -.00001; //-y=(.02)^(1/1.1)
	}
	else if (state == 0) {
		return 0;
	}
	else if (state == 1) {
		return log(getCurSize() + 0.5f) * .00003;
		//return .00004;//.00001; //y=(.02)^(1/1.1)
	}
	else if (state == 2) {
		return log(getCurSize() + 0.5f) * .00005;
		//return .00007;//.00004; //y=(.02)
	}
	else {
		return log(getCurSize() + 0.5f) * .00006;
		//return .0001; //if (state == 3)  //y=(.02)^1.1
	}
}

int UQualities3::updateSize()
{
	int newState = 0;
	if ((light + hunger) / 2 > 500.f) {

		newState = 2;
	}else if ((light + hunger) / 2 > 100.f) {

		newState = 1;
	}
	
	if (newState != sizeState) changeSize = true;
	return newState;
}

float UQualities3::getCurSize() {
	return curSize;
}

float UQualities3::getLargestSize() {
	return largestSize;
}
