// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SteeringGamemode.generated.h"

/**
 * 
 */
UENUM(Blueprintable)
enum SteeringMode { OnePoint, SeveralPoints, Circuit };

UCLASS(Blueprintable)
class STEERING_API ASteeringGamemode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<SteeringMode> Mode;
	
};
