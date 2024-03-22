#include "CameraPlayerPawn.h"
#include "LandscapeProxy.h"
#include "Kismet/GameplayStatics.h"
#include "SteeringGamemode.h"

FActorSpawnParameters SpawnParams;

TArray<AActor*> InsertArrayNextToArray(TArray<AActor*> InsertIn, TArray<AActor*> Inserted);

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
                case SeveralPoints: {
                    TargetsSpawned.Add(GetWorld()->SpawnActor<AActor>(TargetToSpawn, Location, FRotator::ZeroRotator, SpawnParams));
                    FindSeveralPointsPath();
                    break;
                }
                case Circuit: {
                    TargetsSpawned.Add(GetWorld()->SpawnActor<AActor>(TargetToSpawn, Location, FRotator::ZeroRotator, SpawnParams));
                    FindCircuitPath();
                    break;
                }
            }
        }
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
        TArray<AActor*> PathToOnePoint = GetPathBetweenTwoPoints(Vehicule, Target);

        Vehicule->reaching_target = nullptr;
        Vehicule->TargetsToFollow = PathToOnePoint;
    }
}

void ACameraPlayerPawn::FindSeveralPointsPath() {
    for (AVehicule* Vehicule : Vehicules) {
        if (Vehicule->TargetsToFollow.IsEmpty()) {
            Vehicule->OneWayIndex = 0;
            TArray<AActor*> PathBetweenVehiculeAndFirstTarget = GetPathBetweenTwoPoints(Vehicule, TargetsSpawned[0]);
            Vehicule->TargetsToFollow = InsertArrayNextToArray(Vehicule->TargetsToFollow, PathBetweenVehiculeAndFirstTarget);
        }

        if (TargetsSpawned.Num() > 1) {
            TArray<AActor*> PathBetweenTwoLastTargets = GetPathBetweenTwoPoints(TargetsSpawned[TargetsSpawned.Num() - 2], TargetsSpawned.Last());
            Vehicule->TargetsToFollow = InsertArrayNextToArray(Vehicule->TargetsToFollow, PathBetweenTwoLastTargets);
        }
    }
}

void ACameraPlayerPawn::FindCircuitPath() {
    for (AVehicule* Vehicule : Vehicules) {
        Vehicule->PathToFirstTarget = GetPathBetweenTwoPoints(Vehicule, TargetsSpawned[0]);
        Vehicule->HasReachedFirstTarget = false;
        Vehicule->reaching_target = nullptr;
        Vehicule->CircuitTargetIndex = 0;
        Vehicule->OneWayIndex = 0;

        if (TargetsSpawned.Num() > 1) {
            TArray<AActor*> CircuitPath;
            for (int32 i = 0; i < TargetsSpawned.Num(); i++) {
                CircuitPath = InsertArrayNextToArray(CircuitPath, GetPathBetweenTwoPoints(TargetsSpawned[i], TargetsSpawned[(i + 1) % TargetsSpawned.Num()]));
            }
            Vehicule->TargetsToFollow = CircuitPath;
        }
    }
}

TArray<AActor*> ACameraPlayerPawn::GetPathBetweenTwoPoints(AActor* Point1, AActor* Point2) {
    TArray<AActor*> Path;
    ANavNode* ClosestNodeToPoint1 = NearestNode(Point1->GetActorLocation());
    ANavNode* ClosestNodeToPoint2 = NearestNode(Point2->GetTargetLocation());

    TArray<ANavNode*> AStarPath = Nav->AStar(ClosestNodeToPoint1, ClosestNodeToPoint2);
    for (AActor* Node : AStarPath) {
        Path.Add(Node);
    }
    Path.Add(Point2);

    return Path;
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

void ACameraPlayerPawn::ChangingGamemode() {
    ClearTargetsSpawned();
    for (AVehicule* Vehicule : Vehicules) {
        Vehicule->TargetsToFollow.Empty();
        Vehicule->PathToFirstTarget.Empty();
        Vehicule->reaching_target = nullptr;
        Vehicule->HasReachedFirstTarget = false;
        Vehicule->OneWayIndex = 0;
        Vehicule->CircuitTargetIndex = 0;
    }
}

TArray<AActor*> InsertArrayNextToArray(TArray<AActor*> InsertIn, TArray<AActor*> Inserted) {
    TArray<AActor*> FinalArray = InsertIn;
    for (AActor* Element : Inserted) {
        FinalArray.Add(Element);
    }
    return FinalArray;
}

