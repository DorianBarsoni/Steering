#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Controller.h"
#include "CameraPlayerController.generated.h"

UCLASS(Blueprintable)
class STEERING_API ACameraPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACameraPlayerController();
	
};
