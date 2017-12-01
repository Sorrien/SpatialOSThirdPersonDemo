// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSPlayerController.h"
#include "FPSCharacter.h"
#include "RpgDemo.h"



AFPSPlayerController::AFPSPlayerController() {
	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
}

void AFPSPlayerController::PlayerTick(float DeltaTime) {

}

void AFPSPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAxis("MoveForward", this, &AFPSPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AFPSPlayerController::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &AFPSPlayerController::Turn);
	InputComponent->BindAxis("TurnRate", this, &AFPSPlayerController::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &AFPSPlayerController::LookUp);
	InputComponent->BindAxis("LookUpRate", this, &AFPSPlayerController::LookUpAtRate);

}

void AFPSPlayerController::MoveForward(float Value) {
	auto character = Cast<AFPSCharacter>(GetPawn());
	auto Axis = EAxis::X;
	character->SendMoveInput(Value, Axis);
}

void AFPSPlayerController::MoveRight(float Value) {
	auto Axis = EAxis::Y;
	auto character = Cast<AFPSCharacter>(GetPawn());
	character->SendMoveInput(Value, Axis);
}

void AFPSPlayerController::Turn(float Rate)
{
	APawn* const pawn = GetPawn();
	pawn->AddControllerYawInput(Rate);
}

void AFPSPlayerController::LookUp(float Rate)
{
	APawn* const pawn = GetPawn();
	pawn->AddControllerPitchInput(Rate);
}

void AFPSPlayerController::TurnAtRate(float Rate)
{
	Turn(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	//APawn* const pawn = GetPawn();

	// calculate delta for this frame from the rate information
	//pawn->AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AFPSPlayerController::LookUpAtRate(float Rate)
{
	//APawn* const pawn = GetPawn();
	// calculate delta for this frame from the rate information
	LookUp(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	//pawn->AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}