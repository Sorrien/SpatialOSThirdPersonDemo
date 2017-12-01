// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EntityId.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SpatialOS/Public/SpatialOSCommandResult.h"
#include "SpatialOS/Generated/UClasses/HeartbeatSenderComponent.h"
#include "SpatialOS/Generated/UClasses/HeartbeatReceiverComponent.h"
#include "SpatialOS/Generated/UClasses/HeartbeatRequest.h"
#include "SpatialOS/Generated/UClasses/MovementInputReceiverComponent.h"
#include "SpatialOS/Generated/UClasses/MovementInputSenderComponent.h"
#include "SpatialOS/Generated/UClasses/TransformComponent.h"
#include "Commander.h"
#include "FPSCharacter.generated.h"

UCLASS()
class RPGDEMO_API AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSCharacter(const FObjectInitializer& ObjectInitializer);

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable, Category = "MovementInput")
	void OnMovementInputReceiverComponentReady();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	///** Returns TopDownCameraComponent subobject **/
	//FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const
	//{
	//	return TopDownCameraComponent;
	//}
	///** Returns CameraBoom subobject **/
	//FORCEINLINE class USpringArmComponent* GetCameraBoom() const
	//{
	//	return CameraBoom;
	//}

	FORCEINLINE class UPositionComponent* GetPositionComponent() const
	{
		return PositionComponent;
	}

	UFUNCTION(BlueprintCallable, Category = "Heartbeat")
	void OnHeartbeatReceiverComponentReady();
	UFUNCTION(BlueprintCallable, Category = "Heartbeat")
	void OnHeartbeatCommandRequest(class UHeartbeatCommandResponder * Responder);

	/** Returns FollowCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTPCameraComponent() const
	{
		return TPCamera;
	}
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetTPSpringArm() const
	{
		return TPSpringArm;
	}

	FORCEINLINE class UCameraComponent* GetFPCameraComponent() const
	{
		return FPCamera;
	}
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetFPSpringArm() const
	{
		return FPSpringArm;
	}

	FORCEINLINE class UHeartbeatSenderComponent* GetHeartbeatSenderComponent() const
	{
		return HeartbeatSender;
	}

	FORCEINLINE class UHeartbeatReceiverComponent* GetHeartbeatReceiverComponent() const
	{
		return HeartbeatReceiver;
	}

	FORCEINLINE class UTransformComponent* GetTransformComponent() const
	{
		return TransformComponent;
	}

	FORCEINLINE class UMovementInputReceiverComponent* GetMovementInputReceiverComponent() const
	{
		return MovementInputReceiverComponent;
	}

	FORCEINLINE class UMovementInputSenderComponent* GetMovementInputSenderComponent() const
	{
		return MovementInputSenderComponent;
	}

	UFUNCTION(BlueprintPure, Category = "RpgDemoCharacter")
		FEntityId GetEntityId() const;

	UFUNCTION(BlueprintCallable, Category = "MoveInput")
	void SendMoveInput(float Value, EAxis::Type Axis);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable, Category = "MoveInput")
	void OnSendMoveInputCommandRequest(class USendMoveInputCommandResponder* commandResponder);

private:
	void Initialise(EAuthority authority);
	void InitialiseAsOwnPlayer();
	void InitialiseAsOtherPlayer();

	///** Top down camera */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera,
	//	meta = (AllowPrivateAccess = "true"))
	//	class UCameraComponent* TopDownCameraComponent;

	///** Camera boom positioning the camera above the character */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera,
	//	meta = (AllowPrivateAccess = "true"))
	//	class USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera,
		meta = (AllowPrivateAccess = "true"))
		class UDecalComponent* CursorToWorld;

	/** The transform component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RpgDemoCharacter",
		meta = (AllowPrivateAccess = "true"))
		class UPositionComponent* PositionComponent;

	/** The REAL transform component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transform",
		meta = (AllowPrivateAccess = "true"))
		class UTransformComponent* TransformComponent;
	UFUNCTION(BlueprintCallable, Category = "Transform")
	void OnTransformComponentReady();
	UFUNCTION(BlueprintCallable, Category = "Transform")
	void OnTransformComponentUpdate();
	UFUNCTION(BlueprintCallable, Category = "Transform")
	void SendTransform();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input",
		meta = (AllowPrivateAccess = "true"))
		class UMovementInputReceiverComponent* MovementInputReceiverComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input",
		meta = (AllowPrivateAccess = "true"))
		class UMovementInputSenderComponent* MovementInputSenderComponent;

	UFUNCTION(BlueprintCallable, Category = "Position")
		void OnPositionComponentReady();

	UFUNCTION(BlueprintCallable, Category = "Position")
		void OnPositionAuthorityChange(EAuthority newAuthority);
	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* TPSpringArm;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* TPCamera;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* FPSpringArm;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FPCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UHeartbeatSenderComponent* HeartbeatSender;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UHeartbeatReceiverComponent* HeartbeatReceiver;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float TimeSinceLastSendHeartbeat;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		int HeartbeatsMissed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		FHeartbeatCommandResultDelegate OnHeartbeatCommandResponseCallback;
	UFUNCTION(BlueprintCallable, Category = "Heartbeat")
	void OnHeartbeatSenderComponentReady();
	UFUNCTION(BlueprintCallable, Category = "Heartbeat")
	void SendHeartbeat();
	UFUNCTION(BlueprintCallable, Category = "Heartbeat")
	void OnHeartbeatResponseCallback(const struct FSpatialOSCommandResult& Result, class UHeartbeatResponse* Response);
	UFUNCTION(BlueprintCallable, Category = "Heartbeat")
	void OnDeleteEntityResponseCallback(const struct FSpatialOSCommandResult & Result);

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
