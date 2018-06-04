// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ShroommateProtoCharacter.h"
#include "ShroommateProto.h"
#include "SkillTreeController.h"
#include "Interactable.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "MySaveGame.h"
#include <cstdlib>

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Blue,text)


//////////////////////////////////////////////////////////////////////////
// AShroommateProtoCharacter

AShroommateProtoCharacter::AShroommateProtoCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(181.822586f, 369.25589f);
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

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
	CameraBoom->TargetArmLength = 150.0f; // The camera follows at this distance behind the character	
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
	HasKey = false;
	HasBoombox = false;
	HasCD1 = false;
	HasCD2 = false;
	HasCD3 = false;
	HasBattery = false;
	Sec = 0;
	SecTen = 0;
	Min = 0;
	MinTen = 0;
	TPause = false;
	JumpCharge = ischarging;
	JumpChrageTimer = 0;
	WindowCheck = false;
	DoorCheck = false;
	CoinFade = false;
	ObjFade = false;
	CoinAnimation = false;
	ObjAnimation = false;
	KitchenCheck = false;
	BedroomCheck = false;
	SpinachCheck = false;
	SoapCheck = false;
	PepperCheck = false;
	CanEatCheck = true;
	FoodCounter = 15;
	MamaTalkCheck = false;
	RescueBabyCheck = false;
	BabyDropCheck = false;
	MakeNoiseCheck = false;
	RescueGrandCheck = false;
	DropGrandCheck = false;
	KitchenLevelCheck = false;
	BedroomLevelCheck = false;


	//jump setting
	jump_height = 225.f;
	jump_gravity = 0.8f;
	jump_control = 0.5f;

	GetCharacterMovement()->JumpZVelocity = jump_height;
	GetCharacterMovement()->AirControl = jump_control;
	GetCharacterMovement()->GravityScale = jump_gravity;
	GetCharacterMovement()->MaxAcceleration = 350.0f;

	canclimb = false;
	walkagain = false;
	movingW = false;
	movingR = false;

	interacting = false;
	tut1visible = false;
	tut1seen = false;

	glidecheck = true;
	glide = false;

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
	PlayerInputComponent->BindAction("Flatten", IE_Pressed, this, &AShroommateProtoCharacter::Flatten);
	PlayerInputComponent->BindAction("Flatten", IE_Released, this, &AShroommateProtoCharacter::unFlatten);

	PlayerInputComponent->BindAction("Charge", IE_Pressed, this, &AShroommateProtoCharacter::regularJump);
	PlayerInputComponent->BindAction("Charge", IE_Released, this, &AShroommateProtoCharacter::regularJumpStop);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AShroommateProtoCharacter::ShroomCharge);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AShroommateProtoCharacter::ShroomJump);

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

//Inventory system
void AShroommateProtoCharacter::CheckForInteractable() {
	FHitResult HitResult;

	//FVector StartTrace = FollowCamera->GetComponentLocation();
	//FVector EndTrace = (FollowCamera->GetForwardVector() * 200.f) + StartTrace;

	FVector StartTrace = GetActorLocation();
	FVector EndTrace = (GetActorForwardVector() * 50.f) + StartTrace;



	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	ASkillTreeController* Controller = Cast<ASkillTreeController>(GetController());
	if (!Controller) {
		return;
	}

	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_Visibility, QueryParams) && Controller) {
		if (AInteractable* Interactable = Cast<AInteractable>(HitResult.GetActor())) {
			Controller->CurrentInteractable = Interactable;
			return;
		}
	}
	Controller->CurrentInteractable = nullptr;
}


//interaction
void AShroommateProtoCharacter::Interact() {
	a->Interact();
	interacting = true;
}

void AShroommateProtoCharacter::unInteract() {
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
	FRotator BaseRotation = Controller->GetControlRotation();

	camPitchAdjust -= (Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	if(camPitchAdjust > 360){
		camPitchAdjust = 0;
	}else{
		if (camPitchAdjust < 0){
			camPitchAdjust = 360;
		}
	}
	if((camPitchAdjust > 40) && (camPitchAdjust < 178)){
		camPitchAdjust = 40;
	}
	if((camPitchAdjust < 270) && (camPitchAdjust > 200)){
		camPitchAdjust = 270;
	} //NOT NECESSARY IF RELYING ON ORIGINAL CONTROL
	
	if ((BaseRotation.Pitch >0) && (BaseRotation.Pitch < 200) || (BaseRotation.Pitch >= 320)){
		//originalControl = false;
		//This is where cinematic trigger is, stop using player rotation, start using cam rotation and boom length
		//if(rotationTransition == false){
		rotationTransition = true;
		camPitchAdjust = BaseRotation.Pitch;
		if ((camPitchAdjust >40) && (camPitchAdjust < 200)){
			camPitchAdjust = 40;
		}
		//}
		BaseRotation.Pitch = camPitchAdjust;

		if ((BaseRotation.Pitch >0) && (BaseRotation.Pitch < 200) || (BaseRotation.Pitch >= 320)){ //This is the overide for original controls, causes the bump but necessary until entirely flushed out
		//originalControl = false;
		//BaseRotation.Pitch = camPitchAdjust;
		FollowCamera->SetWorldRotation(BaseRotation);
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("%f"), camPitchAdjust));
		}
		//BaseRotation.Pitch = camPitchAdjust;
		//FollowCamera->SetWorldRotation(BaseRotation);
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("%f"), camPitchAdjust));
		////////////////////Boom length piece, 60deg max, 7 deg start?
		if(BaseRotation.Pitch >= 320){
			camBoomAdjust = (-320+BaseRotation.Pitch)/80;
		}else{
			camBoomAdjust = (fabs(40 + BaseRotation.Pitch))/80;
		}
		//camBoomAdjust = (abs(BaseRotation.Pitch - 0))/45;  //35 because max of 40, min of 5, creates a 35 wide range, normalizing to 0 -> 1 value
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::Printf(TEXT("%f"), camBoomAdjust));
		////camBoomAdjust = (camBoomAdjust*camBoomAdjust)/((2*(camBoomAdjust*camBoomAdjust - camBoomAdjust)) + 1);
		camBoomAdjust = (camBoomAdjust*camBoomAdjust)*(3-2*camBoomAdjust);  //easing bezier function for smooth enter/exit
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::Printf(TEXT("%f"), camBoomAdjust));
		camBoomAdjust = (camBoomMax-50)*camBoomAdjust;
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%f"), camBoomAdjust));

		CameraBoom->TargetArmLength = camBoomMax - camBoomAdjust;
		if ((BaseRotation.Pitch < 90) || (BaseRotation.Pitch > 260)){
			originalControl = true;
		}
	}
	if (originalControl == true){
		rotationTransition = false;
		if((BaseRotation.Pitch >0) && (BaseRotation.Pitch < 200) || (BaseRotation.Pitch > 320)){
			CameraBoom->TargetArmLength = camBoomMax - camBoomAdjust;
		}else{
			CameraBoom->TargetArmLength = camBoomMax;
		}
		AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	}
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("%f"), BaseRotation.Pitch));
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("%f"), CameraBoom->TargetArmLength));
}



void AShroommateProtoCharacter::ShroomCharge()
{
	if (jumpEnabled) {
		GetCharacterMovement()->JumpZVelocity = jump_height;

		ischarging = true;
	}
}

void AShroommateProtoCharacter::ShroomJump()
{
	if (jumpEnabled) {

		if (!glide) {
			charge = false;
			JumpCharge = false;
			ischarging = false;
			chargeInterval = 0.f;
			Jump();
		}
		else {
			charge = false;
			JumpCharge = false;
			ischarging = false;
			chargeInterval = 0.f;
			glide = false;
		}

	}
	//GetCharacterMovement()->JumpZVelocity = jump_height;
}

void AShroommateProtoCharacter::regularJump()
{
	if (jumpEnabled) {
		GetCharacterMovement()->JumpZVelocity = jump_height;
		//isJumping = true;
		Jump();
	}
}

void AShroommateProtoCharacter::regularJumpStop()
{
	isJumping = false;
	StopJumping();
}



void AShroommateProtoCharacter::MoveForward(float Value)
{
	if (PepperCheck) {
		Value = 2.f;
	}

	if ((Controller != NULL) && (Value != 0.0f) )
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
			if (Value < 0.0) {
				const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
				AddMovementInput(Direction, Value);
				FVector Force = FVector(0, -500, 0);
				GetCharacterMovement()->AddImpulse(Force);
			}
			else {
				AddMovementInput(Direction, Value);
				FVector Force = FVector(0, 0, 500);
				GetCharacterMovement()->AddImpulse(Force);
			}
		}
		
	}
	else {
		movingW = false;
	}
	
	

	shroomAudio->Play(0.f);
}

void AShroommateProtoCharacter::MoveRight(float Value )
{



	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		movingR = true;
		if (!canclimb) {
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
	}
	else {
		movingR = false;
	}
	if (squishsquish->IsValidLowLevelFast()) {
		shroomAudio->SetSound(squishsquish);
	}

	shroomAudio->Play();
}

void AShroommateProtoCharacter::Flatten(){
	Crouch();
	
	//SetActorRelativeScale3D(oScale * FVector(1.f,1.f,0.5f));
}
void AShroommateProtoCharacter::unFlatten() {
	UnCrouch();
	GetMesh()->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, 0.f));


	//SetActorRelativeScale3D(oScale * FVector(1.f, 1.f, 2.f));
}

// Called every frame
void AShroommateProtoCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckForInteractable();

	
	//Gliding
	if (GetCharacterMovement()->MovementMode == MOVE_Falling) {
		ASkillTreeController* Controller = Cast<ASkillTreeController>(GetController());
		if (Controller != NULL) {
			if (ischarging || isJumping) {
				//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("%f"), GetVelocity().Z));
				if (GetVelocity().Z < -125.0f && glidecheck) {
					GetCharacterMovement()->GravityScale = 0.04f;
					glide = true;
					glidecheck = false;
				}
				if (GetVelocity().Z < -50.0f && glide) {
					FVector Force = FVector(0, 0, 700);
					GetCharacterMovement()->AddImpulse(Force);
				}
			}
			else {
				GetCharacterMovement()->GravityScale = jump_gravity;
				glide = false;
				glidecheck = true;
				if (GetVelocity().Z < 0.0f) {
					FVector Force = FVector(0, 0, -300);
					GetCharacterMovement()->AddImpulse(Force);
				}
			}

		}
	}
	else {
		if (GetCharacterMovement()->GravityScale <= 0.04f) {
			GetCharacterMovement()->GravityScale = jump_gravity;
		}
		//glide = false;
	}



	if (!movingW && !movingR && canclimb) {
		FVector Force = FVector(0, 0, 0);
		GetCharacterMovement()->Velocity = Force;
	}

	if (ischarging && !glide) {
		if (chargeInterval >= 0.125f) {
			if (GetCharacterMovement()->JumpZVelocity <= 325.f) {
				GetCharacterMovement()->JumpZVelocity += chargeLevel;

			}
			chargeInterval = 0.f;
		}
		else {
			chargeInterval += DeltaTime;
		}
		
	}

	if (chargeInterval >= 0.1f) {
		charge = true;
		JumpCharge = true;
	}
	
	if (!charge || glide) {
		GetCharacterMovement()->MaxWalkSpeed = 150.f;
	}
	else {
		GetCharacterMovement()->MaxWalkSpeed = 50.f;
	}

	if (bIsCrouched) {
		GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -260.f));
		GetMesh()->SetRelativeScale3D(FVector(1.f, 1.f, 0.5f));

	}
	else {
		GetMesh()->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
		GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -420.f));
	}
	
	FRotator BaseRotation = Controller->GetControlRotation();
	if ((BaseRotation.Pitch >40) && (BaseRotation.Pitch <200)){
		BaseRotation.Pitch = 40;
		Controller->SetControlRotation(BaseRotation);
	}

	cameraIdle(BaseRotation, previousRotation);
	previousRotation = Controller->GetControlRotation();
	previousActorRotation = this->GetActorRotation();
	if(previousActorRotation.Yaw<0){
		previousActorRotation.Yaw = 360+previousActorRotation.Yaw;
	}

	//Pepper jump
	if (PepperCheck) {

		UGameplayStatics::SpawnEmitterAtLocation(this, smokeFX, GetActorLocation()) ->SetRelativeScale3D(FVector(0.15f,0.15f,0.15f));
		MoveForward(2.f);
		regularJump();
	}
	
}

void AShroommateProtoCharacter::cameraIdle(FRotator currentRotation, FRotator previousRotation){
	if (cameraGuideMode){
		currentDesiredRotation.Yaw = cameraYawOverride;
		currentDesiredRotation.Pitch = cameraPitchOverride;
	}else{
		currentDesiredRotation = this->GetActorRotation();
		if(currentDesiredRotation.Yaw < 0){ //Adjusts controller rotation to be comparable to actor rotation (0->180, -0->-180)
			currentDesiredRotation.Yaw = 360+currentDesiredRotation.Yaw;
		}// now you have two variables, ranged 0 to 360. Actor rotation is changed for ease of moving the cam rotation
	}

	if((currentDesiredRotation != previousActorRotation) && (inIdleTransition == true)){ //player moved character while in transition
		//reset transition variables
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("PLAYER TURNING ACTOR")));
		idleTransitionTimer = 0;
		inIdleTransition = false; //rest of function should reset properly
	}
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("Actor: CurDesRot %f"), currentDesiredRotation.Yaw));
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("Actor: PrevRot %f"), previousActorRotation.Yaw));

	if(currentRotation.Yaw != previousRotation.Yaw){ //reset timer and bool
		idleTimer = 400;
		idleTransitionTimer = 0;
		inIdleTransition = false;
		transitionRange = 0;
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("Not-Idle")));
	}else{
		//continue timer until bool is triggered
		idleTimer--;
		if(idleTimer < 0){
			if(inIdleTransition == false){ //If coming from idle, initialize the transition needs
				inIdleTransition = true;
				startingRotation = currentRotation; //for equation starting point reference
				deltaRotation = currentDesiredRotation.Yaw - currentRotation.Yaw;
				if(deltaRotation < 0){ //if deltaRotation is negative
					if((deltaRotation*-1) > 180){ //if results in wrong choice
						addRotation = true;
						deltaRotation = 360 + deltaRotation; //do other choice
					}else{
						addRotation = false; //do first choice if not worng
						deltaRotation = deltaRotation*-1;
					}
				}else{ //if deltaRotation is positive
					if(deltaRotation > 180){ //if results in wrong choice
						addRotation = false;
						deltaRotation = 360 - deltaRotation; //do other choice
					}else{
						addRotation = true; //do first choice if not wrong
					}
				}
			}

			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("deltaRotation: %f"), deltaRotation));
			if(deltaRotation <=179){
				idleTransitionTimer++;
				if (idleTransitionTimer <= idleTransitionTimerMax){ //while in transition
					transitionRange = idleTransitionTimer/idleTransitionTimerMax;
					transitionRange = sin(3.14159*transitionRange/2);
					//transitionRange = (transitionRange*transitionRange)*(3-2*transitionRange); //new 0->1 range with easing function. Has ease in so don't use
					transitionRange = transitionRange*deltaRotation;
					//now transition range is the new rotation to be added to startingRotation
					//currentRotation.Yaw = (startingRotation.Yaw + transitionRange);
					if (addRotation == true){
						currentRotation.Yaw = (startingRotation.Yaw + transitionRange);
					}else{
						currentRotation.Yaw = (startingRotation.Yaw - transitionRange);
					}	
					if(currentRotation.Yaw < 0){
						currentRotation.Yaw = 360+ currentRotation.Yaw;
					}
					if(currentRotation.Yaw > 360){
						currentRotation.Yaw = currentRotation.Yaw-360;
					}
					Controller->SetControlRotation(currentRotation);
				}
			}

			//Pitch component to camera guide
			if((cameraGuideMode) && (idleTimer < 0)){ //Deciding how to add/subtract deltaRot in order to move properly
				deltaRotation = fabs(currentDesiredRotation.Pitch - currentRotation.Pitch);
				if (deltaRotation >= 180){
					deltaRotation = 360 - deltaRotation;
				}

				if(currentDesiredRotation.Pitch < currentRotation.Pitch){
					if(currentDesiredRotation.Pitch < 269){
						addRotation = true;
					}else{
						addRotation = false;
					}
				}else{ // if(goal > cur)
					if(currentDesiredRotation.Pitch > 269){
						addRotation = true;
					}else{
						addRotation = false;
					}
				}
				if(currentRotation.Pitch < 91){
					addRotation = !addRotation; //shortcut to not write out above statement check again
				}

				if (idleTransitionTimer <= idleTransitionTimerMax){ //while in transition
					transitionRange = idleTransitionTimer/idleTransitionTimerMax;
					transitionRange = sin(3.14159*transitionRange/2); //ease out, not in, due to constant reset if moving
					transitionRange = transitionRange*deltaRotation; //Scaling percent to total movement space
					//now transition range is the new rotation to be added to startingRotation
					//currentRotation.Yaw = (startingRotation.Yaw + transitionRange);
					if (addRotation == true){
						currentRotation.Pitch = (currentRotation.Pitch + transitionRange);
					}else{
						currentRotation.Pitch = (currentRotation.Pitch - transitionRange);
					}	
					if(currentRotation.Pitch < 0){
						currentRotation.Pitch = 360+ currentRotation.Pitch;
					}
					if(currentRotation.Pitch > 360){
						currentRotation.Pitch = currentRotation.Pitch-360;
					}
					Controller->SetControlRotation(currentRotation);
					//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("Actor: CurDesPitchRot %f"), currentDesiredRotation.Pitch));
					//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Actor: deltaRotationPitch %f"), deltaRotation));
					//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("Actor: CurPitchRot %f"), currentRotation.Pitch));
				}
			}

			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("In Transition")));
		} else{
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Idle")));
		}
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

//UI Setter
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
void AShroommateProtoCharacter::setHasKey(bool key) {
	HasKey = key;
}
void AShroommateProtoCharacter::setHasBoombox(bool boombox) {
	HasBoombox = boombox;
}
void AShroommateProtoCharacter::setHasCD1(bool cd1) {
	HasCD1 = cd1;
}
void AShroommateProtoCharacter::setHasCD2(bool cd2) {
	HasCD2 = cd2;
}
void AShroommateProtoCharacter::setHasCD3(bool cd3) {
	HasCD3 = cd3;
}
void AShroommateProtoCharacter::setHasBattery(bool battery) {
	HasBattery = battery;
}
void AShroommateProtoCharacter::setSec(int sec) {
	Sec = sec;
}
void AShroommateProtoCharacter::setSecTen(int secT) {
	SecTen = secT;
}
void AShroommateProtoCharacter::setMin(int min) {
	Min = min;
}
void AShroommateProtoCharacter::setMinTen(int minT) {
	MinTen = minT;
}
void AShroommateProtoCharacter::setTPause(bool TP) {
	TPause = TP;
}
void AShroommateProtoCharacter::setJumpChrageTimer(float JCT) {
	JumpChrageTimer = JCT;
}
void AShroommateProtoCharacter::setWindowCheck(bool WC) {
	WindowCheck = WC;
}
void AShroommateProtoCharacter::setDoorCheck(bool DC) {
	DoorCheck = DC;
}
void AShroommateProtoCharacter::setCoinFade(bool CF) {
	CoinFade = CF;
}
void AShroommateProtoCharacter::setObjFade(bool OF) {
	ObjFade = OF;
}
void AShroommateProtoCharacter::setCoinAnimation(bool CA) {
	CoinAnimation = CA;
}
void AShroommateProtoCharacter::setObjAnimation(bool OA) {
	ObjAnimation = OA;
}
void AShroommateProtoCharacter::setKitchenCheck(bool KC) {
	KitchenCheck = KC;
}
void AShroommateProtoCharacter::setBedroomCheck(bool BC) {
	BedroomCheck = BC;
}
void AShroommateProtoCharacter::setSpinachCheck(bool SC) {
	SpinachCheck = SC;
}
void AShroommateProtoCharacter::setSoapCheck(bool SoapC) {
	SoapCheck = SoapC;
}
void AShroommateProtoCharacter::setPepperCheck(bool PC) {
	PepperCheck = PC;
}
void AShroommateProtoCharacter::setCanEatCheck(bool CEC) {
	CanEatCheck = CEC;
}
void AShroommateProtoCharacter::setFoodCounter(int FC) {
	FoodCounter = FC;
}

void AShroommateProtoCharacter::setMamaTalkCheck(bool MTC) {
	MamaTalkCheck = MTC;
}
void AShroommateProtoCharacter::setRescueBabyCheck(bool RBC) {
	RescueBabyCheck = RBC;
}
void AShroommateProtoCharacter::setBabyDropCheck(bool BDC) {
	BabyDropCheck = BDC;
}
void AShroommateProtoCharacter::setMakeNoiseCheck(bool MNC) {
	MakeNoiseCheck = MNC;
}
void AShroommateProtoCharacter::setRescueGrandCheck(bool RGC) {
	RescueGrandCheck = RGC;
}
void AShroommateProtoCharacter::setDropGrandCheck(bool DGC) {
	DropGrandCheck = DGC;
}
void AShroommateProtoCharacter::setKitchenLevelCheck(bool KLC) {
	KitchenLevelCheck = KLC;
}
void AShroommateProtoCharacter::setBedroomLevelCheck(bool BLC) {
	BedroomLevelCheck = BLC;
}


//UI Getter
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
bool AShroommateProtoCharacter::getHasKey() {
	return HasKey;
}
bool AShroommateProtoCharacter::getHasBoombox() {
	return HasBoombox;
}
bool AShroommateProtoCharacter::getHasCD1() {
	return HasCD1;
}
bool AShroommateProtoCharacter::getHasCD2() {
	return HasCD2;
}
bool AShroommateProtoCharacter::getHasCD3() {
	return HasCD3;
}
bool AShroommateProtoCharacter::getHasBattery() {
	return HasBattery;
}
int AShroommateProtoCharacter::getSec() {
	return Sec;
}
int AShroommateProtoCharacter::getSecTen(){
	return SecTen;
}
int AShroommateProtoCharacter::getMin() {
	return Min;
}
int AShroommateProtoCharacter::getMinTen() {
	return MinTen;
}
bool AShroommateProtoCharacter::getTPause() {
	return TPause;
}
bool AShroommateProtoCharacter::getJumpCharge() {
	//JumpCharge = charge;
	return JumpCharge;
}
float AShroommateProtoCharacter::getJumpChrageTimer() {
	return JumpChrageTimer;
}
bool AShroommateProtoCharacter::getWindowCheck() {
	return WindowCheck;
}
bool AShroommateProtoCharacter::getDoorCheck() {
	return DoorCheck;
}
bool AShroommateProtoCharacter::getCoinFade() {
	return CoinFade;
}
bool AShroommateProtoCharacter::getObjFade() {
	return ObjFade;
}
bool AShroommateProtoCharacter::getCoinAnimation() {
	return CoinAnimation;
}
bool AShroommateProtoCharacter::getObjAnimation() {
	return ObjAnimation;
}
bool AShroommateProtoCharacter::getKitchenCheck() {
	return KitchenCheck;
}
bool AShroommateProtoCharacter::getBedroomCheck() {
	return BedroomCheck;
}
bool AShroommateProtoCharacter::getSpinachCheck() {
	return SpinachCheck;
}
bool AShroommateProtoCharacter::getSoapCheck() {
	return SoapCheck;
}
bool AShroommateProtoCharacter::getPepperCheck() {
	return PepperCheck;
}
bool AShroommateProtoCharacter::getCanEatCheck() {
	return CanEatCheck;
}
int AShroommateProtoCharacter::getFoodCounter() {
	return FoodCounter;
}
bool AShroommateProtoCharacter::getMamaTalkCheck() {
	return MamaTalkCheck;
}
bool AShroommateProtoCharacter::getRescueBabyCheck() {
	return RescueBabyCheck;
}
bool AShroommateProtoCharacter::getBabyDropCheck() {
	return BabyDropCheck;
}
bool AShroommateProtoCharacter::getMakeNoiseCheck() {
	return MakeNoiseCheck;
}
bool AShroommateProtoCharacter::getRescueGrandCheck() {
	return RescueGrandCheck;
}
bool AShroommateProtoCharacter::getDropGrandCheck() {
	return DropGrandCheck;
}
bool AShroommateProtoCharacter::getKitchenLevelCheck() {
	return KitchenLevelCheck;
}
bool AShroommateProtoCharacter::getBedroomLevelCheck() {
	return BedroomLevelCheck;
}