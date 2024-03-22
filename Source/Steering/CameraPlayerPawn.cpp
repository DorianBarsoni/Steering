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
        PathToFollow.Empty();
        PathToFollow = GetPathBetweenTwoPoints(Vehicule, Target);

        Vehicule->reaching_target = nullptr;
        Vehicule->TargetsToFollow = PathToFollow;
    }
}

void ACameraPlayerPawn::FindSeveralPointsPath() {
    PathToFollow.Empty();

    for (AVehicule* Vehicule : Vehicules) {
        for (AActor* Target : TargetsSpawned) {
            if (PathToFollow.IsEmpty()) {
                PathToFollow = GetPathBetweenTwoPoints(Vehicule, Target);
            }
            else {
                PathToFollow = InsertArrayNextToArray(PathToFollow, GetPathBetweenTwoPoints(PathToFollow.Last(), Target));
            }
        }
        Vehicule->TargetsToFollow = PathToFollow;
        PathToFollow.Empty();
    }
}

void ACameraPlayerPawn::FindCircuitPath() {
    PathToFollow.Empty();

    for (AVehicule* Vehicule : Vehicules) {
        for (AActor* Target : TargetsSpawned) {
            if (PathToFollow.IsEmpty()) {
                PathToFollow = GetPathBetweenTwoPoints(Vehicule, Target);
            }
            else {
                PathToFollow = InsertArrayNextToArray(PathToFollow, GetPathBetweenTwoPoints(PathToFollow.Last(), Target));
            }
        }
        PathToFollow = InsertArrayNextToArray(PathToFollow, GetPathBetweenTwoPoints(PathToFollow.Last(), PathToFollow[0]));
        Vehicule->TargetsToFollow = PathToFollow;
        PathToFollow.Empty();
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
        Vehicule->reaching_target = nullptr;
    }
}

TArray<AActor*> InsertArrayNextToArray(TArray<AActor*> InsertIn, TArray<AActor*> Inserted) {
    TArray<AActor*> FinalArray = InsertIn;
    for (AActor* Element : Inserted) {
        FinalArray.Add(Element);
    }
    return FinalArray;
}

