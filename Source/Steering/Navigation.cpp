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
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("%d"), Nodes.Num()));

	if (ShowGraph) {
		DrawGraph();
	}
	
}

void ANavigation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

TArray<ANavNode*> ANavigation::AStar(ANavNode* Start, ANavNode* End) {
	ResetNodeCostAndPredecessor();

	TArray<ANavNode*> NodesToProcess;
	NodesToProcess.Add(Start);
	Start->Predecesor = Start;
	Start->Cost = 0;

	ANavNode* NodeToProcess;
	while (!NodesToProcess.IsEmpty()) {
		NodeToProcess = NodesToProcess.Pop();
		if (NodeToProcess != End) {
			for (auto Neighbor : NodeToProcess->LinkedNodes) {
				if (Neighbor->Cost == -1 || CalculateCost(NodeToProcess, Neighbor, End) < Neighbor->Cost) {
					Neighbor->Cost = NodeToProcess->Cost + SquaredDistanceBetweenTwoNodes(NodeToProcess, Neighbor);
					Neighbor->Predecesor = NodeToProcess;
					NodesToProcess.Add(Neighbor);
				}
			}
		}
	}

	ANavNode* CurrentNode = End;
	TArray<ANavNode*> Path;
	Path.Add(End);
	//FString Way = Path[0]->Name;
	while (CurrentNode != Start) {
		CurrentNode = CurrentNode->Predecesor;
		Path.Insert(CurrentNode, 0);
		//Way = Path[0]->Name + " -> " + Way;
	}

	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, Way);
	return Path;
}

float ANavigation::SquaredDistanceBetweenTwoNodes(ANavNode* Node1, ANavNode* Node2) {
	return FVector::DistSquared(Node1->GetActorLocation(), Node2->GetActorLocation());
}

float ANavigation::Heuristic(ANavNode* Node1, ANavNode* Node2) {
	return FVector::DistSquared(Node1->GetActorLocation(), Node2->GetActorLocation());
}

float ANavigation::CalculateCost(ANavNode* Node1, ANavNode* Node2, ANavNode* End) {
	return Node1->Cost + SquaredDistanceBetweenTwoNodes(Node1, Node2) + Heuristic(Node2, End);
}

void ANavigation::ResetNodeCostAndPredecessor() {
	for (ANavNode* Node : Nodes) {
		Node->Cost = -1;
		Node->Predecesor = nullptr;
	}
}

void ANavigation::DrawGraph() {
	for (ANavNode* Node : Nodes) {
		DrawDebugSphere(GetWorld(), Node->GetActorLocation(), 60.0f, 10, FColor::Cyan, true);
		for (ANavNode* Neighbour : Node->LinkedNodes) {
			DrawDebugLine(GetWorld(), Node->GetActorLocation(), Neighbour->GetActorLocation(), FColor::Green, true, 1.0, 0U, 10.0f);
		}
	}
}

