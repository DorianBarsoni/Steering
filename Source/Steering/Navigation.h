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
	void AStar(ANavNode* Start, ANavNode* End);

};
