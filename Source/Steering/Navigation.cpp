// Fill out your copyright notice in the Description page of Project Settings.


#include "Navigation.h"

// Sets default values
ANavigation::ANavigation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ANavigation::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANavigation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ANavigation::AStar(ANavNode* Start, ANavNode* End) {
	TArray<ANavNode*> NodesToProcess;
	NodesToProcess.Add(Start);
	Start->Predecesor = Start;

	ANavNode* NodeToProcess;
	while (!NodesToProcess.IsEmpty()) {
		NodeToProcess = NodesToProcess.Pop();
		if (NodeToProcess != End) {
			for (auto Neighbor : NodeToProcess->LinkedNodes) {
				//ATTENTION COST INITIALISE A 0
				if (NodeToProcess->Cost + FVector::DistSquared(NodeToProcess->GetActorLocation(), Neighbor->GetActorLocation()) < Neighbor->Cost) {
					Neighbor->Cost = NodeToProcess->Cost + FVector::DistSquared(NodeToProcess->GetActorLocation(), Neighbor->GetActorLocation());
					Neighbor->Predecesor = NodeToProcess;
					NodesToProcess.Add(Neighbor);
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, NodeToProcess->Name + " -> " + Neighbor->Name);
				}
			}
		}
	}

	ANavNode* CurrentNode = End;
	FString Way = End->Name;
	/*while (CurrentNode != Start) {
		CurrentNode = CurrentNode->Predecesor;
		Way = CurrentNode->Name + " -> " + Way;
	}*/

	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, Way);
}

