#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NavNode.generated.h"

UCLASS(Blueprintable)
class STEERING_API ANavNode : public AActor
{
	GENERATED_BODY()
	
public:	
	ANavNode();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ANavNode*> LinkedNodes;

	ANavNode* Predecesor = nullptr;

	float Cost = -1;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
