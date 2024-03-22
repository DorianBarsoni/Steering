#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NavNode.h"
#include "Navigation.generated.h"

UCLASS(Blueprintable)
class STEERING_API ANavigation : public AActor
{
	GENERATED_BODY()
	
public:	
	ANavigation();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ANavNode*> Nodes;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	TArray<ANavNode*> AStar(ANavNode* Start, ANavNode* End);

	UPROPERTY(EditAnywhere)
	bool ShowGraph;

	void DrawGraph();

private:
	float SquaredDistanceBetweenTwoNodes(ANavNode* Node1, ANavNode* Node2);

	float Heuristic(ANavNode* Node1, ANavNode* Node2);

	float CalculateCost(ANavNode* Node1, ANavNode* Node2, ANavNode* End);

	void ResetNodeCostAndPredecessor();

};
