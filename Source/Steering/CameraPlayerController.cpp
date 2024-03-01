// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraPlayerController.h"

ACameraPlayerController::ACameraPlayerController() {
	PrimaryActorTick.bCanEverTick = true;

	bShowMouseCursor = true;
	bEnableClickEvents = true;
}

