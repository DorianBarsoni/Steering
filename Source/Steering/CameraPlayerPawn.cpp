#include "CameraPlayerPawn.h"
#include "LandscapeProxy.h"

ACameraPlayerPawn::ACameraPlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;

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
    bool res = TraceLineFromCameraToMousePosition(HitResult, true);

    FVector Location = HitResult.ImpactPoint;

    if (res) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("%f %f %f"), Location.X, Location.Y, Location.Z));
    if (res) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, HitResult.GetActor()->GetName());
}

bool ACameraPlayerPawn::TraceLineFromCameraToMousePosition(FHitResult& HitResult, bool showHit) {
    APlayerController* PlayerController = Cast<APlayerController>(GetController());

    FVector MouseWorldPosition, MouseWorldDirection;
    PlayerController->DeprojectMousePositionToWorld(MouseWorldPosition, MouseWorldDirection);
    FVector Direction = MouseWorldPosition + 10000.0 * MouseWorldDirection;

    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(this);
    CollisionParams.AddIgnoredActor(GetController());

    FCollisionObjectQueryParams ObjectParams;
    ObjectParams.AddObjectTypesToQuery(ECC_WorldStatic);

    if (GetWorld()->LineTraceSingleByObjectType(HitResult, MouseWorldPosition, Direction, ObjectParams, CollisionParams)) {
        if (showHit && HitResult.GetActor() && HitResult.GetActor()->IsA<ALandscapeProxy>()) {
            DrawDebugLine(GetWorld(), MouseWorldPosition, Direction, FColor::Red, false, 5.0f, 0, 0.1f);
            return true;
        }
    }
    return false;
}

