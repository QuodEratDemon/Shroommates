// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "Components/ActorComponent.h"
#include "Qualities3.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SHROOMMATEPROTO_API UQualities3 : public UActorComponent
{
	GENERATED_BODY()

        //vars (Going to have these be 0-1 for now)
    UPROPERTY(EditAnywhere, Category = "Qualities")
        float light;
    UPROPERTY(EditAnywhere, Category = "Qualities")
        float hunger;
    UPROPERTY(EditAnywhere, Category = "Qualities")
        float humidity;

	UPROPERTY(EditAnywhere)
		float threshold = .1; //AG 10/14/17: Adding threshold for growthRate()
	UPROPERTY(EditAnywhere)
		float decayRate = .00001; //AG 10/14/17: Decays qualities over time

    UPROPERTY(EditAnywhere, Category = "Qualities")
		float curSize; //AG 10/14/17: player's current size
	UPROPERTY(EditAnywhere, Category = "Qualities")
		float largestSize = 0; //AG 10/14/17: player's largest size ever

	float timeTick = 0.f;

public:
	// Sets default values for this component's properties
	UQualities3();

protected:

	//AG 10/14/17: Protected vars
	AActor * player; //will be used to access actor attached to this script

					 // Called when the game starts
	virtual void BeginPlay() override;

	//AG 10/14/17: Helper function for growthRate()
	int qualityState(float q);

	//AG 10/16/17: Helper function for growthRate()
	int qualityHunger(float q);

	//AG 10/16/17: Helper function for printing. Stand-in for having UI
	void printSelect(FString t, int state);

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//My functions---------------------------------------------------------------

	//Adds to them (can use negative numbers for subtracting)
	float addToLight(float change);
	float addToHunger(float change);
	float addToHumidity(float change);

	//setters
	float setLight(float newLight);
	float setHunger(float newTemp);
	float setHumidity(float newHumidity);

    //getters
	UFUNCTION(BlueprintPure, Category = "Qualities")
	float getLight();
	UFUNCTION(BlueprintPure, Category = "Qualities")
	float getHunger();
	UFUNCTION(BlueprintPure, Category = "Qualities")
	float getHumidity();
	UFUNCTION(BlueprintPure, Category = "Qualities")
	float getCurSize();
	UFUNCTION(BlueprintPure, Category = "Qualities")
	float getLargestSize();

	//AG 10/14/17: Adding function that determines if player should grow or shrink and by how much
	float growthRate();



};
