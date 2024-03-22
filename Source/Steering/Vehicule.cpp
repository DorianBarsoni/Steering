#include "Vehicule.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetMathLibrary.h"
#include "MathUtil.h"

AVehicule::AVehicule() : mass(1), velocity(0), max_force(10), max_speed(10) {
	PrimaryActorTick.bCanEverTick = true;
	SwitchWay = false;
	
}

void AVehicule::BeginPlay() {
	Super::BeginPlay();
	position = this->GetActorLocation();

	SteeringGM = Cast<ASteeringGamemode>(GetWorld()->GetAuthGameMode());
	
}

void AVehicule::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	Move();

}

void AVehicule::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

FVector AVehicule::truncate(FVector v, float value) {
	if (v.Size() > value) {
		return v.GetSafeNormal() * value;
	}
	//UE_LOG(LogTemp, Warning, TEXT("New velocity : %s"), *(v).ToString());
	return v;
}

void AVehicule::calculateNewPosition(FVector steering_direction) {
	FVector steering_force = truncate(steering_direction, max_force);
	FVector acceleration = steering_force / mass;
	velocity = truncate(velocity + acceleration, max_speed);
	position = position + velocity;
}

void AVehicule::caculateNewOrientation() {
	orientation.SetColumn(0, velocity.GetSafeNormal());
}

void AVehicule::updatePositionAndRotationAccordingToSteering(FVector steering) {
	calculateNewPosition(steering);
	caculateNewOrientation();

	this->SetActorLocation(position);
	float theta = UKismetMathLibrary::Acos(orientation.GetColumn(0).X);
	theta = theta * 180 / PI;
	if (orientation.GetColumn(0).Y < 0) {
		theta *= -1;
	}
	this->SetActorRotation(FRotator(0, theta, 0));
}

void AVehicule::seek(AActor *target) {
	FVector desired_velocity = (target->GetActorLocation() - this->GetActorLocation()).GetSafeNormal() * max_speed;
	FVector steering = desired_velocity - velocity;

	updatePositionAndRotationAccordingToSteering(steering);
}

void AVehicule::flee(AActor* target) {
	FVector desired_velocity = (target->GetActorLocation() - this->GetActorLocation()).GetSafeNormal() * -max_speed;
	FVector steering = desired_velocity - velocity;

	updatePositionAndRotationAccordingToSteering(steering);
}

void AVehicule::pursuit(AVehicule *target, float c) {
	float distance = (GetActorLocation() - target->GetActorLocation()).Size()/100.0;
	UE_LOG(LogTemp, Warning, TEXT("Distance entre objets : %f"), distance);

	FVector furtur_location = target->GetActorLocation() + target->velocity*distance*c;

	FVector desired_velocity = (furtur_location - this->GetActorLocation()).GetSafeNormal() * max_speed;
	FVector steering = desired_velocity - velocity;

	updatePositionAndRotationAccordingToSteering(steering);
}

void AVehicule::evade(AVehicule* target, float c) {
	float distance = (GetActorLocation() - target->GetActorLocation()).Size() / 100.0;
	UE_LOG(LogTemp, Warning, TEXT("Distance entre objets : %f"), distance);

	FVector furtur_location = target->GetActorLocation() + target->velocity * distance * c;

	FVector desired_velocity = (target->GetActorLocation() - this->GetActorLocation()).GetSafeNormal() * -max_speed;
	FVector steering = desired_velocity - velocity;

	updatePositionAndRotationAccordingToSteering(steering);
}

void AVehicule::arrival(AActor *target, float slowing_distance) {
	FVector target_offset = target->GetActorLocation() - GetActorLocation();
	float distance = target_offset.Size();

	float ramped_speed = max_speed * (distance / slowing_distance);
	float clipped_speed = FMath::Min(ramped_speed, max_speed);

	FVector desired_velocity = (clipped_speed / distance) * target_offset;
	FVector steering = desired_velocity - velocity;

	updatePositionAndRotationAccordingToSteering(steering);
}

void AVehicule::circuit(TArray<AActor*> targets) {
	if (!targets.IsEmpty()) {
		if (!reaching_target) {
			reaching_target = targets[0];
		}

		seek(reaching_target);

		if ((GetActorLocation() - reaching_target->GetActorLocation()).Size() < 100) {
			if (reaching_target->IsA<AVehicule>()) {
				HasReachedFirstTarget = true;
			}

			ReachingTargetIndex++;
			reaching_target = targets[ReachingTargetIndex % targets.Num()];
		}	
	}
}

bool AVehicule::OneWay(TArray<AActor*> targets) {
	if (!targets.IsEmpty()) {
		if (!reaching_target) {
			reaching_target = targets[0];
			ReachingTargetIndex = 0;
		}

		if (ReachingTargetIndex == targets.Num() - 1) {
			arrival(reaching_target, 1000);
			if ((GetActorLocation() - reaching_target->GetActorLocation()).Size() < 100)
				return true;
		}
		else {
			seek(reaching_target);
			if ((GetActorLocation() - reaching_target->GetActorLocation()).Size() < 100) {
				ReachingTargetIndex++;
				reaching_target = targets[ReachingTargetIndex];
			}
		}
		return false;
	}
	return true;
}

bool AVehicule::ReverseWay(TArray<AActor*> targets) {
	if (!targets.IsEmpty()) {
		if (!reaching_target) {
			reaching_target = targets[targets.Num() - 1];
		}

		int index = -1;
		for (int i = 0; i < targets.Num(); ++i) {
			if (targets[i] == reaching_target) {
				index = i;
				break;
			}
		}
		seek(reaching_target);

		if (index == 0) {
			if ((GetActorLocation() - reaching_target->GetActorLocation()).Size() < 100)
				return true;
		}
		else {
			if ((GetActorLocation() - reaching_target->GetActorLocation()).Size() < 100) {
				index--;
				reaching_target = targets[index];
			}
		}
		return false;
	}
	return true;
}

void AVehicule::TwoWays() {
	if (!TargetsToFollow.IsEmpty()) {
		if (!SwitchWay) {
			SwitchWay = OneWay(TargetsToFollow);
		}
		else {
			SwitchWay = !ReverseWay(TargetsToFollow);
		}
	}
}

void AVehicule::Move() {
	if (SteeringGM) {
		if (!TargetsToFollow.IsEmpty()) {
			switch (SteeringGM->Mode) {
				case OnePoint: {
					OneWay(TargetsToFollow);
					break;
				}
				case SeveralPoints: {
					OneWay(TargetsToFollow);
					break;
				}
				case Circuit: {
					if (HasReachedFirstTarget) {
						circuit(TargetsToFollow);
					}
					else {
						HasReachedFirstTarget = OneWay(PathToFirstTarget);
					}
					break;
				}
			}
		}
	}
}



