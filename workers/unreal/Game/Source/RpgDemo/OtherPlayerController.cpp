// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#include "OtherPlayerController.h"

#include "PositionComponent.h"
#include "RpgDemo.h"
#include "RpgDemoCharacter.h"
#include "FPSCharacter.h"
#include "SpatialOSConversionFunctionLibrary.h"

AOtherPlayerController::AOtherPlayerController()
{
}

void AOtherPlayerController::Possess(APawn* InPawn)
{
    Super::Possess(InPawn);
    //mOtherPlayer = Cast<ARpgDemoCharacter>(InPawn);
	mOtherPlayer = Cast<AFPSCharacter>(InPawn);
    mOtherPlayer->GetPositionComponent()->OnCoordsUpdate.AddDynamic(
        this, &AOtherPlayerController::OnPositionUpdate);
}

void AOtherPlayerController::UnPossess()
{
    Super::UnPossess();
    mOtherPlayer->GetPositionComponent()->OnCoordsUpdate.RemoveDynamic(
        this, &AOtherPlayerController::OnPositionUpdate);
}

void AOtherPlayerController::OnPositionUpdate()
{
    SetNewMoveDestination(mOtherPlayer->GetPositionComponent()->Coords);
}

void AOtherPlayerController::SetNewMoveDestination(const FVector& DestLocation)
{
    UNavigationSystem* const NavSys = GetWorld()->GetNavigationSystem();
    float const Distance = FVector::Dist(DestLocation, mOtherPlayer->GetActorLocation());
    if (NavSys)
    {
        NavSys->SimpleMoveToLocation(this, DestLocation);
    }
}
