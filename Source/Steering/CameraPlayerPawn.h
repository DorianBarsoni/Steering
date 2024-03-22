#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Vehicule.h"
#include "NavNode.h"
#include "Navigation.h"
#include "CameraPlayerPawn.generated.h"

UCLASS(Blueprintable)
class STEERING_API ACameraPlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	ACameraPlayerPawn();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> TargetToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> VehiculeType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ANavigation> BPNav;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> TargetsSpawned;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AVehicule*> Vehicules;

	ANavigation* Nav;

	TArray<AActor*> PathToFollow;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UFUNCTION(BlueprintCallable)
	void CreateTarget();

	bool TraceLineFromCameraToMousePosition(FHitResult& HitResult);

	void FindOnePointPath();

	void FindSeveralPointsPath();

	void FindCircuitPath();

	TArray<AActor*> GetPathBetweenTwoPoints(AActor* Point1, AActor* Point2);

	UFUNCTION(BlueprintCallable)
	void ChangingGamemode();

private:
	ANavNode* NearestNode(FVector Location);

	bool IsCloserThan(FVector Vehicule, FVector ActualNearestNode, FVector Node);

	void ClearTargetsSpawned();

};
