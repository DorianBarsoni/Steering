#include "CameraPlayerPawn.h"
#include "LandscapeProxy.h"
#include "Kismet/GameplayStatics.h"
#include "SteeringGamemode.h"

FActorSpawnParameters SpawnParams;

ACameraPlayerPawn::ACameraPlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

}

void ACameraPlayerPawn::BeginPlay()
{
	Super::BeginPlay();

    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), VehiculeType, FoundActors);

    for (AActor* FoundActor : FoundActors) {
        if (AVehicule* Vehicule = Cast<AVehicule>(FoundActor)) {
            Vehicules.Add(Vehicule);
        }
    }

    AActor* Actor;
    Actor = UGameplayStatics::GetActorOfClass(GetWorld(), BPNav);
    Nav = Cast<ANavigation>(Actor);
    if(!Nav) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Navigation not set in Player");
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
    if (TraceLineFromCameraToMousePosition(HitResult)) {
        FVector Location = HitResult.ImpactPoint;
        Location.Z = 380.0f;

        ASteeringGamemode* SteeringGM = Cast<ASteeringGamemode>(GetWorld()->GetAuthGameMode());
        if (SteeringGM) {
            switch (SteeringGM->Mode) {
                case OnePoint: {
                    ClearTargetsSpawned();
                    TargetsSpawned.Add(GetWorld()->SpawnActor<AActor>(TargetToSpawn, Location, FRotator::ZeroRotator, SpawnParams));
                    FindOnePointPath();
                    break;
                }
            }
        }
        //SetVehiculesTargets();
    }
}

bool ACameraPlayerPawn::TraceLineFromCameraToMousePosition(FHitResult& HitResult) {
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
        if (HitResult.GetActor() && HitResult.GetActor()->IsA<ALandscapeProxy>()) {
            return true;
        }
        else {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, HitResult.GetActor()->GetName());
        }
    }
    return false;
}

void ACameraPlayerPawn::FindOnePointPath() {
AActor* Target = TargetsSpawned[0];

    for (AVehicule* Vehicule : Vehicules) {
        PathToFollow.Empty();

        ANavNode* ClosestNodeToVehicule = NearestNode(Vehicule->GetActorLocation());
        ANavNode* ClosestNodeToTarget = NearestNode(Target->GetTargetLocation());

        TArray<ANavNode*> AStarPath = Nav->AStar(ClosestNodeToVehicule, ClosestNodeToTarget);
        for (AActor* Node : AStarPath) {
            PathToFollow.Add(Node);
        }
        PathToFollow.Add(Target);

        Vehicule->reaching_target = nullptr;
        Vehicule->TargetsToFollow = PathToFollow;
    }
    //PathToFollow = Nav->AStar(ClosestNode, )
}

void ACameraPlayerPawn::SetVehiculesTargets() {
    for (auto Vehicule : Vehicules) {
        ANavNode* ClosestNode;
        if (PathToFollow.IsEmpty()) {
            ClosestNode = NearestNode(Vehicule->GetActorLocation());
        }
        else {
            ClosestNode = NearestNode(PathToFollow.Last()->GetActorLocation());
        }
         

        //Il faut séparer le trouvage de chemin quand on ajoute un point du chemin initial

        for (auto Target : TargetsSpawned) {
            ANavNode* ClosestNodeToTarget = NearestNode(Target->GetActorLocation());

            TArray<ANavNode*> AStarPath = Nav->AStar(ClosestNode, ClosestNodeToTarget);

            for (AActor* Node : AStarPath) {
                PathToFollow.Add(Node);
            }
            PathToFollow.Add(Target);

            ClosestNode = ClosestNodeToTarget;
        }
        //Vehicule->TargetsToFollow = PathToFollow;
    }
}

ANavNode* ACameraPlayerPawn::NearestNode(FVector Location) {
    ANavNode* NNode = nullptr;

    if (Nav && !Nav->Nodes.IsEmpty()) {
        NNode = Nav->Nodes[0];
        for (auto Node : Nav->Nodes) {
            if (IsCloserThan(Location, NNode->GetActorLocation(), Node->GetActorLocation())) {
                NNode = Node;
            }
        }
    }

    return NNode;
}

bool ACameraPlayerPawn::IsCloserThan(FVector Vehicule, FVector ActualNearestNode, FVector Node) {
    return FVector::DistSquared(Vehicule, Node) < FVector::DistSquared(Vehicule, ActualNearestNode);
}

void ACameraPlayerPawn::ClearTargetsSpawned() {
    for (int32 Index = 0; Index < TargetsSpawned.Num(); Index++) {
        TargetsSpawned[Index]->Destroy();
    }
    TargetsSpawned.Empty();
}

