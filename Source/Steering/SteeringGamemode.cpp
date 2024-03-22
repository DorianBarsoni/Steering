// Fill out your copyright notice in the Description page of Project Settings.


#include "SteeringGamemode.h"

void ASteeringGamemode::OnePointMode() {
	Mode = OnePoint;
}

void ASteeringGamemode::SeveralPointsMode() {
	Mode = SeveralPoints;
}

void ASteeringGamemode::CircuitMode() {
	Mode = Circuit;
}