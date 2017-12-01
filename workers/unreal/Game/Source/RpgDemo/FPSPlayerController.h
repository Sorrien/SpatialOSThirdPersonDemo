// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEMO_API AFPSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AFPSPlayerController();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;
	
protected:
	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface

	UFUNCTION(BlueprintCallable, Category = "Input")
	void Turn(float Rate);
	UFUNCTION(BlueprintCallable, Category = "Input")
	void LookUp(float Rate);
	UFUNCTION(BlueprintCallable, Category = "Input")
	void TurnAtRate(float Rate);
	UFUNCTION(BlueprintCallable, Category = "Input")
	void LookUpAtRate(float Rate);

	UFUNCTION(BlueprintCallable, Category = "Input")
	void MoveForward(float Value);
	UFUNCTION(BlueprintCallable, Category = "Input")
	void MoveRight(float Value);
};
