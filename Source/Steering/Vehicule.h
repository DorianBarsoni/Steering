// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Math/Vector.h"
#include "SteeringGamemode.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> TargetsToFollow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor *reaching_target;

	ASteeringGamemode* SteeringGM;

public:
	AVehicule();

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	FVector truncate(FVector v, float value);

	UFUNCTION()
	void calculateNewPosition(FVector steering_direction);

	UFUNCTION()
	void caculateNewOrientation();

	UFUNCTION()
	void updatePositionAndRotationAccordingToSteering(FVector steering);

	bool SwitchWay;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void seek(AActor *target);

	UFUNCTION(BlueprintCallable)
	void flee(AActor *target);

	UFUNCTION(BlueprintCallable)
	void pursuit(AVehicule *target, float c);

	UFUNCTION(BlueprintCallable)
	void evade(AVehicule* target, float c);

	UFUNCTION(BlueprintCallable)
	void arrival(AActor *target, float slowing_distance);

	UFUNCTION(BlueprintCallable)
	void circuit(TArray<AActor*> targets);

	UFUNCTION(BlueprintCallable)
	bool OneWay(TArray<AActor*> targets);

	UFUNCTION(BlueprintCallable)
	bool ReverseWay(TArray<AActor*> targets);

	UFUNCTION(BlueprintCallable)
	void TwoWays();

	UFUNCTION(BlueprintCallable)
	void Move();
};
