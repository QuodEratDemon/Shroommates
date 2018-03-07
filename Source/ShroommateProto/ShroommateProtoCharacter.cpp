// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ShroommateProtoCharacter.h"
#include "SkillTreeController.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "MySaveGame.h"


#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Blue,text)


//////////////////////////////////////////////////////////////////////////
// AShroommateProtoCharacter

AShroommateProtoCharacter::AShroommateProtoCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	//GetCharacterMovement()->JumpZVelocity = 600.f;
	//GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 2000.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	//sound stuff
	static ConstructorHelpers::FObjectFinder<USoundCue>squish(TEXT("'/Game/walking'"));
	squishsquish = squish.Object;

	shroomAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("walkingComponent"));
	shroomAudio->bAutoActivate = false;
	shroomAudio->SetupAttachment(RootComponent);

	//AG 10/22/17: Clambor point stuff
	onWall = false;
	justJumped = false;
	wallRate = 0.05f;
	timeSinceWallJump = 40.0f;

	//Skill tree stuff
	skillpoints = 6;
	agility1 = false;
	agility2 = false;
	agility3 = false;
	jump1 = false;
	jump2 = false;
	jump3 = false;

	//UI
	MaxHealth = 3;
	CurrentHealth = 3;
	MaxWater = 100;
	CurrentWater = 70;
	Currency = 0;
	State = 2;
	MaxExp = 15;
	CurrentExp = 0;
	Level = 1;
	FoundShroom = false;


	//jump setting
	jump_height = 3000.f;
	jump_gravity = 10.0f;
	jump_control = 0.2f;

	GetCharacterMovement()->JumpZVelocity = jump_height;
	GetCharacterMovement()->AirControl = jump_control;
	GetCharacterMovement()->GravityScale = jump_gravity;


	canclimb = false;
	walkagain = false;
	movingW = false;
	movingR = false;

	interacting = false;
	tut1visible = false;
	tut1seen = false;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AShroommateProtoCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Save", IE_Pressed, this, &AShroommateProtoCharacter::SaveGame);
	PlayerInputComponent->BindAction("Load", IE_Pressed, this, &AShroommateProtoCharacter::LoadGame);
	PlayerInputComponent->BindAction("OpenSkillTree", IE_Pressed, this, &AShroommateProtoCharacter::OpenSkillTree);
	PlayerInputComponent->BindAction("OpenStore", IE_Pressed, this, &AShroommateProtoCharacter::OpenStore);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AShroommateProtoCharacter::Interact);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &AShroommateProtoCharacter::unInteract);


	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AShroommateProtoCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShroommateProtoCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AShroommateProtoCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AShroommateProtoCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AShroommateProtoCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AShroommateProtoCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AShroommateProtoCharacter::OnResetVR);

	if (squishsquish->IsValidLowLevelFast()) {
		shroomAudio->SetSound(squishsquish);
	}



}

//Does stuff at "on start"
void AShroommateProtoCharacter::BeginPlay()
{
	Super::BeginPlay();
	//casting pointer to SkillTreeController reference
	a = Cast<ASkillTreeController>(Controller);
}

//interaction
void AShroommateProtoCharacter::Interact() {
	print("pressed");
	interacting = true;

}

void AShroommateProtoCharacter::unInteract() {
	print("release");
	interacting = false;

}

void AShroommateProtoCharacter::setInteract(bool in)
{
	interacting = in;
}

bool AShroommateProtoCharacter::getInteract() {
	return interacting;

}

//Calls SkilltreeController function when you press U-key
void AShroommateProtoCharacter::OpenSkillTree()
{
	//if a is not NULL, it calls BeginPlay() in SKillTreeController and it spawns the skill tree UI
	if (a) {
		a->accessskill();
	}
}
//Calls SkilltreeController function when you press E-key
void AShroommateProtoCharacter::OpenStore()
{
	//if a is not NULL, it calls accessstore() in SKillTreeController and it spawns the store UI
	if (a) {
		a->accessstore();
	}
}

void AShroommateProtoCharacter::SetClimb(bool b)
{
	canclimb = b;
}



void AShroommateProtoCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AShroommateProtoCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	/*
	GetCharacterMovement()->JumpZVelocity = jump_height;
	GetCharacterMovement()->AirControl = jump_control;
	GetCharacterMovement()->GravityScale = jump_gravity;
	*/

	Jump();
}

void AShroommateProtoCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AShroommateProtoCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AShroommateProtoCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AShroommateProtoCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		movingW = true;
		// find out which way is forward
		if (!canclimb) {
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);
			// get forward vector
			if (walkagain) {
				GetCharacterMovement()->SetMovementMode(MOVE_Walking);
				walkagain = false;
			}
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			if (onWall) Value = 0;
			AddMovementInput(Direction, Value);
		}
		else if(canclimb){
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(-1, Rotation.Yaw, 0);
			// get up vector
			if (!walkagain) {
				GetCharacterMovement()->SetMovementMode(MOVE_Flying);
				walkagain = true;
			}
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Z);
			AddMovementInput(Direction, Value);
			FVector Force = FVector(0, 0, 500);
			GetCharacterMovement()->AddImpulse(Force);
		}
		
	}
	else {
		movingW = false;
	}
	
	

	shroomAudio->Play(0.f);
}

void AShroommateProtoCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		movingR = true;
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		// get right vector 
		if (walkagain) {
			GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			walkagain = false;
		}
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		if (onWall) Value = 0;
		AddMovementInput(Direction, Value);
	}
	else {
		movingR = false;
	}
	if (squishsquish->IsValidLowLevelFast()) {
		shroomAudio->SetSound(squishsquish);
	}

	shroomAudio->Play();
}

// Called every frame
void AShroommateProtoCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!movingW && !movingR && canclimb) {
		FVector Force = FVector(0, 0, 0);
		GetCharacterMovement()->Velocity = Force;
	}

	//AG 10/22/17: Get on wall
	/*if (canWall && timeSinceWallJump >= wallRate) {
		APlayerController* Controller = GetWorld()->GetFirstPlayerController();
		if (Controller != NULL) {
			if (Controller->IsInputKeyDown(EKeys::SpaceBar)) {
				GetCharacterMovement()->SetMovementMode(MOVE_Flying);
				GetCharacterMovement()->StopMovementImmediately();
				onWall = true;
				justJumped = false;
			}
		}
	}*/
	
	//AG 10/22/17: Jump off wall
	if (canWall && timeSinceWallJump >= wallRate){//(canWall && !justJumped) || (canWall && timeSinceWallJump >= wallRate)) {
		APlayerController* Controller = GetWorld()->GetFirstPlayerController();
		if (Controller != NULL) {
			if (Controller->IsInputKeyDown(EKeys::SpaceBar)) {
				GetCharacterMovement()->SetMovementMode(MOVE_Walking);
				Jump();
				onWall = false;
				justJumped = true;
				timeSinceWallJump = 0;
				canWall = false;
				//print("can Jump");
			}
		}
	}

	//AG 10/22/17: Reset timer for when jumped off wall
	if (justJumped && timeSinceWallJump < wallRate) {
		timeSinceWallJump += DeltaTime;
	}
	else {
		justJumped = false;
	}
}

void AShroommateProtoCharacter::SaveGame()
{
	//instance of the savegame class
	UMySaveGame* SaveGameInstance = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));
	//Set the save game instance location equal to the players current location
	SaveGameInstance->PlayerLocation = this->GetActorLocation();
	//Save the savegameinstance
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("MySlot"), 0);
	//debug
	//GEngine->AddOnScreenDebugMessage(-1, 5f, FColor:Red, TEXT("Game Saved"));
}

void AShroommateProtoCharacter::LoadGame()
{
	//instance of the savegame class
	UMySaveGame* SaveGameInstance = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));
	//Load the saved game into our savegameinstance variable
	SaveGameInstance = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot("MySlot", 0));
	//set the players position from the saved game file
	this->SetActorLocation(SaveGameInstance->PlayerLocation);
	//debug
	//GEngine->AddOnScreenDebugMessage(-1, 5f, FColor:Red, TEXT("Game Loaded"));
}

void AShroommateProtoCharacter::setMaxHealth(float MH) {
	MaxHealth = MH;
}
void AShroommateProtoCharacter::setCurrentHealth(float CH) {
	CurrentHealth = CH;
}
void AShroommateProtoCharacter::setMaxWater(float MW) {
	MaxWater = MW;
}
void AShroommateProtoCharacter::setCurrentwater(float CW) {
	CurrentWater = CW;
}
void AShroommateProtoCharacter::setCurrency(int C) {
	Currency = C;
}
void AShroommateProtoCharacter::setState(int S) {
	State = S;
}
void AShroommateProtoCharacter::setMaxExp(float ME) {
	MaxExp = ME;
}
void AShroommateProtoCharacter::setCurrentExp(float CE) {
	CurrentExp = CE;
}
void AShroommateProtoCharacter::setLevel(int L) {
	Level = L;
}
void AShroommateProtoCharacter::setFoundShroom(bool FS) {
	FoundShroom = FS;
}


float AShroommateProtoCharacter::getMaxHealth() {
	return MaxHealth;
}
float AShroommateProtoCharacter::getCurrentHealth() {
	return CurrentHealth;
}
float AShroommateProtoCharacter::getMaxWater() {
	return MaxWater;
}
float AShroommateProtoCharacter::getCurrentWater() {
	return CurrentWater;
}
int AShroommateProtoCharacter::getCurrency() {
	return Currency;
}
int AShroommateProtoCharacter::getState() {
	return State;
}
float AShroommateProtoCharacter::getMaxExp() {
	return MaxExp;
}
float AShroommateProtoCharacter::getCurrentExp() {
	return CurrentExp;
}
int AShroommateProtoCharacter::getLevel() {
	return Level;
}
bool AShroommateProtoCharacter::getFoundShroom() {
	return FoundShroom;
}