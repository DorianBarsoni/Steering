#include "CameraPlayerPawn.h"
#include "LandscapeProxy.h"

FActorSpawnParameters SpawnParams;

ACameraPlayerPawn::ACameraPlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

}

void ACameraPlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACameraPlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACameraPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ACameraPlayerPawn::CreateTarget() {
    FHitResult HitResult;
    if (TraceLineFromCameraToMousePosition(HitResult, true)) {
        FVector Location = HitResult.ImpactPoint;
        GetWorld()->SpawnActor<AActor>(TargetToSpawn, Location, FRotator::ZeroRotator, SpawnParams);
    }
}

bool ACameraPlayerPawn::TraceLineFromCameraToMousePosition(FHitResult& HitResult, bool showHit) {
    APlayerController* PlayerController = Cast<APlayerController>(GetController());

    FVector MouseWorldPosition, MouseWorldDirection;
    PlayerController->DeprojectMousePositionToWorld(MouseWorldPosition, MouseWorldDirection);
    FVector Direction = MouseWorldPosition + 20000.0 * MouseWorldDirection;

    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(this);
    CollisionParams.AddIgnoredActor(GetController());

    FCollisionObjectQueryParams ObjectParams;
    ObjectParams.AddObjectTypesToQuery(ECC_WorldStatic);

    if (GetWorld()->LineTraceSingleByObjectType(HitResult, MouseWorldPosition, Direction, ObjectParams, CollisionParams)) {
        if (showHit && HitResult.GetActor() && HitResult.GetActor()->IsA<ALandscapeProxy>()) {
            DrawDebugLine(GetWorld(), MouseWorldPosition, Direction, FColor::Red, true, 5.0f, 0, 0.1f);
            return true;
        }
        else {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, HitResult.GetActor()->GetName());
        }
    }
    return false;
}

