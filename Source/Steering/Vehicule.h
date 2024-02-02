// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Math/Vector.h"
#include "Vehicule.generated.h"

UCLASS(Blueprintable)
class STEERING_API AVehicule : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	float mass;
	FVector position;
	UPROPERTY(EditAnywhere)
	FVector velocity;
	UPROPERTY(EditAnywhere)
	FMatrix orientation;
	UPROPERTY(EditAnywhere)
	float max_force;
	UPROPERTY(EditAnywhere)
	int max_speed;

public:
	AVehicule();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	FVector truncate(FVector v, float value);

	UFUNCTION()
	void calculateNewPosition(FVector steering_direction);

	UFUNCTION()
	void caculateNewOrientation();

	UFUNCTION(BlueprintCallable)
	void seek(AActor *target);

	UFUNCTION(BlueprintCallable)
	void flee(AActor* target);
};
