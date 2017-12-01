// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSCharacter.h"

#include "EntityRegistry.h"
#include "OtherPlayerController.h"
#include "PositionComponent.h"
#include "RPGDemoGameInstance.h"
#include "RpgDemo.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "SpatialOS.h"
#include "EntityId.h"
#include "EntityTemplate.h"
#include "Commander.h"
#include "improbable/standard_library.h"
#include "FPSPlayerController.h"
#include "SpatialMovementComponent.h"

// Sets default values
AFPSCharacter::AFPSCharacter(const FObjectInitializer& ObjectInitializer) //: Super(ObjectInitializer.SetDefaultSubobjectClass<USpatialMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;  // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	//CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	//CameraBoom->SetupAttachment(RootComponent);
	//CameraBoom->bAbsoluteRotation = true;  // Don't want arm to rotate when character does
	//CameraBoom->TargetArmLength = 800.f;
	//CameraBoom->RelativeRotation = FRotator(-60.f, 0.f, 0.f);
	//CameraBoom->bDoCollisionTest =
	//	false;  // Don't want to pull camera in when it collides with level

	//			// Create a camera...
	//TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	//TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	//TopDownCameraComponent->bUsePawnControlRotation = false;  // Camera does not rotate relative to arm

	TPSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("TPSpringArm"));
	TPSpringArm->SetupAttachment(RootComponent);
	TPSpringArm->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	TPSpringArm->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	TPSpringArm->bEnableCameraLag = true;
	//TPSpringArm->bEditableWhenInherited = false;

	// Create a follow camera
	TPCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TPCamera"));
	TPCamera->SetupAttachment(TPSpringArm, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	TPCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	FPSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("FPSpringArm"));
	FPSpringArm->SetupAttachment(GetMesh());
	FPSpringArm->TargetArmLength = 0.0f; // The camera follows at this distance behind the character	
	FPSpringArm->bUsePawnControlRotation = true; // Rotate the arm based on the controller

												 // Create a follow camera
	FPCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FPCamera"));
	FPCamera->SetupAttachment(FPSpringArm, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FPCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	FPCamera->FieldOfView = 100.0f;
	FPCamera->bAutoActivate = false;

	// Create a decal in the world to show the cursor's location
	CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(
		TEXT("Material'/Game/TopDownCPP/Blueprints/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
	}
	CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());

	// Deactivate by default
	CursorToWorld->SetActive(false);
	CursorToWorld->SetHiddenInGame(true);

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// Create a transform component
	PositionComponent = CreateDefaultSubobject<UPositionComponent>(TEXT("PositionComponent"));
	HeartbeatReceiver = CreateDefaultSubobject<UHeartbeatReceiverComponent>(TEXT("HeartbeatReceiver"));
	HeartbeatSender = CreateDefaultSubobject<UHeartbeatSenderComponent>(TEXT("HeartbeatSender"));
	TransformComponent = CreateDefaultSubobject<UTransformComponent>(TEXT("TransformComponent"));
	MovementInputReceiverComponent = CreateDefaultSubobject<UMovementInputReceiverComponent>(TEXT("MovementInputReceiverComponent"));
	MovementInputSenderComponent = CreateDefaultSubobject<UMovementInputSenderComponent>(TEXT("MovementInputSenderComponent"));

	
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	InitialiseAsOtherPlayer();
	PositionComponent->OnAuthorityChange.AddDynamic(this,
		&AFPSCharacter::OnPositionAuthorityChange);
	PositionComponent->OnComponentReady.AddDynamic(this,
		&AFPSCharacter::OnPositionComponentReady);
	//if (HeartbeatReceiver->GetAuthority() == EAuthority::Authoritative || HeartbeatReceiver->GetAuthority() == EAuthority::AuthorityLossImminent) {
		HeartbeatReceiver->OnComponentReady.AddDynamic(this,
			&AFPSCharacter::OnHeartbeatReceiverComponentReady);
	//}
	//if (HeartbeatSender->GetAuthority() == EAuthority::Authoritative || HeartbeatSender->GetAuthority() == EAuthority::AuthorityLossImminent) {
		HeartbeatSender->OnComponentReady.AddDynamic(this,
			&AFPSCharacter::OnHeartbeatSenderComponentReady);
	//}
	MovementInputReceiverComponent->OnComponentReady.AddDynamic(this,
		&AFPSCharacter::OnMovementInputReceiverComponentReady);
	TransformComponent->OnComponentReady.AddDynamic(this,
		&AFPSCharacter::OnTransformComponentReady);

	OnHeartbeatCommandResponseCallback.BindUFunction(this, "OnHeartbeatResponseCallback");
}

// Called every frame
void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PositionComponent->GetAuthority() == EAuthority::Authoritative || PositionComponent->GetAuthority() == EAuthority::AuthorityLossImminent)
	{
		PositionComponent->Coords = GetActorLocation();
	}
	if (HeartbeatSender->GetAuthority() == EAuthority::Authoritative || HeartbeatSender->GetAuthority() == EAuthority::AuthorityLossImminent) {
		TimeSinceLastSendHeartbeat += DeltaTime;
		if (TimeSinceLastSendHeartbeat >= 1.0f) {
			TimeSinceLastSendHeartbeat = 0.0f;
			SendHeartbeat();
		}
	}
	if (TransformComponent->GetAuthority() == EAuthority::Authoritative || TransformComponent->GetAuthority() == EAuthority::AuthorityLossImminent) {
	SendTransform();
	}
	else {
		if (TransformComponent->IsComponentReady()) {
			auto rotation = TransformComponent->Rotation;
			FRotator rot = FRotator(rotation->GetPitch(), rotation->GetYaw(), rotation->GetRoll());
			FVector LerpPos = FMath::VInterpConstantTo(GetActorLocation(), TransformComponent->Location, DeltaTime, 2);
			FRotator LerpRot = FMath::RInterpConstantTo(GetActorRotation(), rot, DeltaTime, 2);

			SetActorLocation(LerpPos);
			SetActorRotation(LerpRot);
		}
	}
}

void AFPSCharacter::OnHeartbeatSenderComponentReady() {

	/*OnHeartbeatCommandResponseCallback.BindUFunction(this, "OnHeartbeatResponseCallback");*/
}

void AFPSCharacter::OnTransformComponentReady() {
	SetActorLocation(TransformComponent->Location);
	TransformComponent->OnComponentUpdate.AddDynamic(this,
		&AFPSCharacter::OnTransformComponentUpdate);
}

void AFPSCharacter::OnTransformComponentUpdate() {
	//if (TransformComponent->GetAuthority() != EAuthority::Authoritative && TransformComponent->IsComponentReady()) {
	//	auto rotation = TransformComponent->Rotation;
	//	FRotator rot = FRotator(rotation->GetPitch(), rotation->GetYaw(), rotation->GetRoll());
	//	FQuat quat = rot.Quaternion();
	//	SetActorRotation(quat, ETeleportType::None);
	//	FHitResult* result = nullptr;
	//	SetActorLocation(TransformComponent->Location, false, result, ETeleportType::None);

	//	
	//	//GetCharacterMovement()->Velocity = TransformComponent->Velocity;
	//}
}

void AFPSCharacter::SendTransform() {
	if (TransformComponent->GetAuthority() == EAuthority::Authoritative || TransformComponent->GetAuthority() == EAuthority::AuthorityLossImminent) {
		auto actorRotation = GetActorRotation();
		auto actorLocation = GetActorLocation();
		auto actorVelocity = GetCharacterMovement()->Velocity;
		auto rotation = improbable::transform::Rotation(actorRotation.Pitch, actorRotation.Yaw, actorRotation.Roll);
		auto location = improbable::Vector3f(actorLocation.X, actorLocation.Y, actorLocation.Z);
		auto velocity = improbable::Vector3f(actorVelocity.X, actorVelocity.Y, actorVelocity.Z);
		auto updateRaw = improbable::transform::Transform::Update();
		updateRaw.set_location(location);
		updateRaw.set_rotation(rotation);
		updateRaw.set_velocity(velocity);
		auto update = NewObject<UTransformComponentUpdate>()->Init(updateRaw);

		TransformComponent->SendComponentUpdate(update);
	}
}

void AFPSCharacter::OnMovementInputReceiverComponentReady() {
	MovementInputReceiverComponent->OnSendMoveInputCommandRequest.AddDynamic(this,
		&AFPSCharacter::OnSendMoveInputCommandRequest);
}
void AFPSCharacter::OnHeartbeatReceiverComponentReady() {
	HeartbeatReceiver->OnHeartbeatCommandRequest.AddDynamic(this,
		&AFPSCharacter::OnHeartbeatCommandRequest);
}

void AFPSCharacter::OnHeartbeatCommandRequest(class UHeartbeatCommandResponder* Responder) {
	auto rawResponse = improbable::player::HeartbeatResponse();
	auto response = NewObject<UHeartbeatResponse>()->Init(rawResponse);
	Responder->SendResponse(response);
}

void AFPSCharacter::SendHeartbeat() {
	auto heartbeatRequest = NewObject<UHeartbeatRequest>()->Init(improbable::player::HeartbeatRequest());
	auto entityId = GetEntityId();
	HeartbeatSender->SendCommand()->Heartbeat(entityId, heartbeatRequest, OnHeartbeatCommandResponseCallback, 0, ECommandDelivery::SHORT_CIRCUIT);
}

void AFPSCharacter::OnHeartbeatResponseCallback(const struct FSpatialOSCommandResult& Result, class UHeartbeatResponse* Response) {

	if (Result.Success()) {
		HeartbeatsMissed = 0;
	}
	else {
		HeartbeatsMissed++;
	}
	if (HeartbeatsMissed >= 15) {
		auto commander = HeartbeatSender->SendCommand();
		auto entityId = GetEntityId();
		FDeleteEntityResultDelegate callback;
		callback.BindUFunction(this, "OnDeleteEntityResponseCallback");
		commander->DeleteEntity(entityId, callback, 0);
	}
}

void AFPSCharacter::OnDeleteEntityResponseCallback(const struct FSpatialOSCommandResult & Result) {

}

// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &AFPSCharacter::Turn);
	PlayerInputComponent->BindAxis("TurnRate", this, &AFPSCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &AFPSCharacter::LookUp);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AFPSCharacter::LookUpAtRate);
}

void AFPSCharacter::MoveForward(float Value) {
	auto Axis = EAxis::X;
	SendMoveInput(Value, Axis);
}

void AFPSCharacter::MoveRight(float Value) {
	auto Axis = EAxis::Y;
	SendMoveInput(Value, Axis);
}

void AFPSCharacter::Turn(float Rate)
{
	AddControllerYawInput(Rate);
}

void AFPSCharacter::LookUp(float Rate)
{
	AddControllerPitchInput(Rate);
}

void AFPSCharacter::TurnAtRate(float Rate)
{
	Turn(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AFPSCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	LookUp(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AFPSCharacter::OnPositionAuthorityChange(EAuthority newAuthority)
{
	Initialise(newAuthority);
}

void AFPSCharacter::OnPositionComponentReady()
{
	//SetActorLocation(PositionComponent->Coords);
}

/** If this is our player, then possess it with the player controller and activate the camera and
*the cursor,
*	otherwise, add an OtherPlayerController */
void AFPSCharacter::Initialise(EAuthority authority)
{
	if (authority == EAuthority::Authoritative || authority == EAuthority::AuthorityLossImminent)
	{
		InitialiseAsOwnPlayer();
		UE_LOG(LogTemp, Warning,
			TEXT("ARpgDemoCharacter::Initialise did just call InitialiseAsOwnPlayer"
				"controller for actor %s"),
			*GetName())
	}
	else
	{
		InitialiseAsOtherPlayer();
		UE_LOG(LogTemp, Warning,
			TEXT("ARpgDemoCharacter::Initialise did just call InitialiseAsOtherPlayer"
				"controller for actor %s"),
			*GetName())
	}
}

void AFPSCharacter::InitialiseAsOwnPlayer()
{
	APlayerController* playerController =  GetWorld()->GetFirstPlayerController();
	if (playerController == nullptr)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("ARpgDemoCharacter::InitialiseAsOwnPlayer error, playerController was null"))
			return;
	}
	AController* currentController = GetController();
	if (currentController != playerController)
	{
		if (currentController != nullptr)
		{
			currentController->UnPossess();
		}

		if (playerController->GetPawn() != nullptr)
		{
			playerController->UnPossess();
		}
		playerController->Possess(this);
		UE_LOG(LogTemp, Warning,
			TEXT("ARpgDemoCharacter::InitialiseAsOwnPlayer creating own player "
				"controller for actor %s"),
			*GetName())
	}

	//if (!CursorToWorld->IsActive())
	//{
	//	CursorToWorld->SetActive(true);
	//}
	//CursorToWorld->SetHiddenInGame(false);

	if (!TPCamera->IsActive())
	{
		TPCamera->SetActive(true);
		TPSpringArm->SetActive(true);
	}
}

void AFPSCharacter::InitialiseAsOtherPlayer()
{
	AController* currentController = GetController();
	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	if (currentController == playerController && playerController != nullptr)
	{
		playerController->UnPossess();
		currentController = GetController();
	}

	if (currentController == nullptr)
	{
		AOtherPlayerController* otherPlayerController = Cast<AOtherPlayerController>(
			GetWorld()->SpawnActor(AOtherPlayerController::StaticClass()));

		UE_LOG(LogTemp, Warning,
			TEXT("ARpgDemoCharacter::InitialiseAsOtherPlayer creating other player "
				"controller for actor %s"),
			*GetName())
			otherPlayerController->Possess(this);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
			TEXT("ARpgDemoCharacter::InitialiseAsOtherPlayer controller was not null"
				"controller for actor %s"),
			*GetName())
	}

	//if (CursorToWorld->IsActive())
	//{
	//	CursorToWorld->SetActive(false);
	//}
	//CursorToWorld->SetHiddenInGame(true);

	if (TPCamera->IsActive())
	{
		TPCamera->SetActive(false);
	}
	if (FPCamera->IsActive())
	{
		FPCamera->SetActive(false);
	}
}

FEntityId AFPSCharacter::GetEntityId() const
{
	auto GameInstance = Cast<URPGDemoGameInstance>(GetWorld()->GetGameInstance());

	if (GameInstance != nullptr)
	{
		auto EntityRegistry = GameInstance->GetEntityRegistry();
		if (EntityRegistry != nullptr)
		{
			return EntityRegistry->GetEntityIdFromActor(this);
		}
	}

	return FEntityId();
}

void AFPSCharacter::SendMoveInput(float Value, EAxis::Type Axis)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		if (MovementInputSenderComponent->IsComponentReady() && MovementInputSenderComponent->GetAuthority() == EAuthority::Authoritative) {
			// find out which way is right
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get right vector 
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(Axis);

			AddMovementInput(Direction, Value);

			//send command with float and direction to server

			improbable::Vector3f directionSpatial = improbable::Vector3f(Direction.X, Direction.Y, Direction.Z);
			auto entityId = GetEntityId();
			auto moveInput = NewObject<UMoveInput>()->Init(improbable::player::MoveInput(Value, directionSpatial));
			const FSendMoveInputCommandResultDelegate callback;
			MovementInputSenderComponent->SendCommand()->SendMoveInput(entityId, moveInput, callback, 0, ECommandDelivery::ROUNDTRIP);
			//auto inputEvent = improbable::player::MoveInput(Value, directionSpatial);
			//auto rawUpdate = improbable::player::MovementInput::Update().add_move_input_added(inputEvent);
			//auto update = NewObject<UMovementInputComponentUpdate>()->Init(rawUpdate);			
			//movementInputComponent->SendComponentUpdate(update);
		}
	}
}

void AFPSCharacter::OnSendMoveInputCommandRequest(class USendMoveInputCommandResponder* commandResponder) {
	if (MovementInputReceiverComponent->GetAuthority() == EAuthority::Authoritative) {
		auto request = commandResponder->GetRequest();
		auto moveInputValue = request->GetValue();
		auto moveInputDirection = request->GetDirection();
		AddMovementInput(moveInputDirection, moveInputValue);
	}
}