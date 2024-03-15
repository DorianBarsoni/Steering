#include "Navigation.h"
#include "Kismet/GameplayStatics.h"

ANavigation::ANavigation()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ANavigation::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANavNode::StaticClass(), FoundActors);

	for (AActor* FoundActor : FoundActors) {
		if (ANavNode* Node = Cast<ANavNode>(FoundActor)) {
			Nodes.Add(Node);
		}
	}
	
}

void ANavigation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

TArray<ANavNode*> ANavigation::AStar(ANavNode* Start, ANavNode* End) {
	TArray<ANavNode*> NodesToProcess;
	NodesToProcess.Add(Start);
	Start->Predecesor = Start;
	Start->Cost = 0;

	ANavNode* NodeToProcess;
	while (!NodesToProcess.IsEmpty()) {
		NodeToProcess = NodesToProcess.Pop();
		if (NodeToProcess != End) {
			for (auto Neighbor : NodeToProcess->LinkedNodes) {
				if (NodeToProcess->Cost + CalculateCost(NodeToProcess, Neighbor) < Neighbor->Cost || Neighbor->Cost == -1) {
					Neighbor->Cost = NodeToProcess->Cost + FVector::DistSquared(NodeToProcess->GetActorLocation(), Neighbor->GetActorLocation());
					Neighbor->Predecesor = NodeToProcess;
					NodesToProcess.Add(Neighbor);
				}
			}
		}
	}

	ANavNode* CurrentNode = End;
	TArray<ANavNode*> Path;
	Path.Add(End);
	FString Way = Path[0]->Name;
	while (CurrentNode != Start) {
		CurrentNode = CurrentNode->Predecesor;
		Path.Insert(CurrentNode, 0);
		Way = Path[0]->Name + " -> " + Way;
	}

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, Way);
	return Path;
}

float ANavigation::SquaredDistanceBetweenTwoPoints(FVector Point1, FVector Point2) {
	return FVector::DistSquared(Point1, Point2);
}

float ANavigation::Heuristic(FVector Point1, FVector Point2) {
	return FVector::DistSquared(Point1, Point2);
}

float ANavigation::CalculateCost(ANavNode* Node1, ANavNode* Node2) {
	return /*Heuristic(Node1->GetActorLocation(), Node2->GetActorLocation()) +*/
		SquaredDistanceBetweenTwoPoints(Node1->GetActorLocation(), Node2->GetActorLocation());
}

