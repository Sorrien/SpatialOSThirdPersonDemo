// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#pragma once

#include "AIController.h"
#include "RpgDemoCharacter.h"
#include "FPSCharacter.h"
#include "OtherPlayerController.generated.h"

/**
 *
 */
UCLASS()
class RPGDEMO_API AOtherPlayerController : public AAIController
{
    GENERATED_BODY()

  public:
    AOtherPlayerController();
    virtual void Possess(APawn* InPawn) override;
    virtual void UnPossess() override;

  protected:
    UFUNCTION(BlueprintCallable, Category = "OtherPlayerController")
    void OnPositionUpdate();

    void SetNewMoveDestination(const FVector& DestLocation);

    AFPSCharacter* mOtherPlayer;
};
