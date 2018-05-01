// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "GameFramework/Character.h"
#include "Engine/DataTable.h"
#include "ShroommateProtoCharacter.generated.h"

USTRUCT(BlueprintType)
struct FCraftingInfo : public FTableRowBase {

	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ComponentID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ProductID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDestroyItemA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDestroyItemB;
};


USTRUCT(BlueprintType)
struct FInventoryItem : public FTableRowBase {

	GENERATED_BODY()

public:

	FInventoryItem() {
		Name = FText::FromString("Item");
		Action = FText::FromString("Use");
		Description = FText::FromString("Enter description for this item");
		Value = 10;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class APickUp> ItemPickUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Action;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Value;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Thumbnail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCraftingInfo> CraftCombinations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanBeUsed;

	bool operator == (const FInventoryItem& Item) const {
		if (ItemID == Item.ItemID)
			return true;
		else return false;
	}

};

UCLASS(config=Game)
class AShroommateProtoCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	AShroommateProtoCharacter();

	float camPitchAdjust = 0.f;
	bool rotationTransition = false;
	bool originalControl = true;
	float camBoomMax = 2000;
	float camBoomAdjust = 0;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(Category = sound, EditAnywhere)
	class UAudioComponent* shroomAudio;

	UPROPERTY(Category = sound, EditAnywhere)
	class USoundCue* squishsquish;

	//AG 10/22/17: Clambor point stuff
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	bool onWall;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	bool justJumped;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float wallRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float timeSinceWallJump;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	bool canWall = false;

	//Skill Tree stuff
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		int skillpoints;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool agility1;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool agility2;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool agility3;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool jump1;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool jump2;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool jump3;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		class ASkillTreeController* a;

	//UI stuff
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		float MaxHealth;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		float CurrentHealth;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		float MaxWater;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		float CurrentWater;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		int Currency;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		int State;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		float MaxExp;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		float CurrentExp;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		int Level;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool FoundShroom;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool HasKey;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool HasBoombox;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool HasCD1;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool HasCD2;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool HasCD3;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool HasBattery;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		int Sec;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		int SecTen;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		int Min;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		int MinTen;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool TPause;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool JumpCharge;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		float JumpChrageTimer;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool WindowCheck;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool DoorCheck;


	//jump
	float jump_gravity;
	float jump_height;
	float jump_control;

	bool charge = false;
	bool ischarging = false;
	float chargeLevel = 500.f;
	float chargeInterval = 0.f;

	bool isJumping = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool canclimb;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool walkagain;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool movingW;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool movingR;

	//Inventory system
	

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool interacting;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool tut1visible;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool tut1seen;

	//Gliding
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool glidecheck;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
		bool glide;

	void Interact();
	void unInteract();


protected:

	virtual void BeginPlay() override;

	void CheckForInteractable();

	//Flatten
	void Flatten();
	void unFlatten();

	//Calls SkilltreeController function when you press U-key
	void OpenSkillTree();

	//Calls SkilltreeController function when you press E-key
	void OpenStore();

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	void ShroomCharge();
	void ShroomJump();
	void regularJump();
	void regularJumpStop();

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

private:
	void SaveGame();
	void LoadGame();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetClimb(bool b);

	//UI setter
	void setMaxHealth(float MH);
	void setCurrentHealth(float CH);
	void setMaxWater(float MW);
	void setCurrentwater(float CW);
	void setCurrency(int C);
	void setState(int S);
	void setMaxExp(float ME);
	void setCurrentExp(float CE);
	void setLevel(int L);
	void setFoundShroom(bool FS);
	void setHasKey(bool key);
	void setHasBoombox(bool boombox);
	void setHasCD1(bool cd1);
	void setHasCD2(bool cd2);
	void setHasCD3(bool cd3);
	void setHasBattery(bool battery);
	void setSec(int sec);
	void setSecTen(int secT);
	void setMin(int min);
	void setMinTen(int minT);
	void setTPause(bool TP);
	void setJumpCharge(bool JC);
	void setJumpChrageTimer(float JCT);
	void setWindowCheck(bool WC);
	void setDoorCheck(bool DC);

	void setInteract(bool in);
	bool getInteract();

	// UI Getter
	float getMaxHealth();
	float getCurrentHealth();
	float getMaxWater();
	float getCurrentWater();
	int getCurrency();
	int getState();
	float getMaxExp();
	float getCurrentExp();
	int getLevel();
	bool getFoundShroom();
	bool getHasKey();
	bool getHasBoombox();
	bool getHasCD1();
	bool getHasCD2();
	bool getHasCD3();
	bool getHasBattery();
	int getSec();
	int getSecTen();
	int getMin();
	int getMinTen();
	bool getTPause();
	bool getJumpCharge();
	float getJumpChrageTimer();
	bool getWindowCheck();
	bool getDoorCheck();

};

