// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicule.h"
#include "Math/UnrealMathUtility.h"

// Sets default values
AVehicule::AVehicule()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AVehicule::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVehicule::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AVehicule::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AVehicule::truncate(FVector v, float value) {
	return FMath::Min(v.Size(), value);
}

