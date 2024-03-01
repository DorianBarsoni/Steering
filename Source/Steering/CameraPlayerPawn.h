#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CameraPlayerPawn.generated.h"

UCLASS(Blueprintable)
class STEERING_API ACameraPlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	ACameraPlayerPawn();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> TargetToSpawn;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UFUNCTION(BlueprintCallable)
	void CreateTarget();

	bool TraceLineFromCameraToMousePosition(FHitResult& HitResult, bool showHit);

};
