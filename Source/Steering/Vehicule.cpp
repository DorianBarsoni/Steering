#include "Vehicule.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetMathLibrary.h"
#include "MathUtil.h"

AVehicule::AVehicule() : mass(1), velocity(0), max_force(10), max_speed(10) {
	PrimaryActorTick.bCanEverTick = true;
	
}

void AVehicule::BeginPlay() {
	Super::BeginPlay();
	position = this->GetActorLocation();
	UE_LOG(LogTemp, Warning, TEXT("GetActorLocation : %s"), *(this->GetActorLocation()).ToString());
	UE_LOG(LogTemp, Warning, TEXT("Position : %s"), *position.ToString());
	
}

void AVehicule::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

void AVehicule::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

FVector AVehicule::truncate(FVector v, float value) {
	if (v.Size() > value) {
		//UE_LOG(LogTemp, Warning, TEXT("New velocity : %s"), *(v.GetSafeNormal() * value).ToString());
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
	UE_LOG(LogTemp, Warning, TEXT("Velocity : %s"), *(velocity).ToString());
	UE_LOG(LogTemp, Warning, TEXT("Velocity normal : %s"), *(velocity.GetSafeNormal()).ToString());
	orientation.SetColumn(0, velocity.GetSafeNormal());
	UE_LOG(LogTemp, Warning, TEXT("Column 0 : %s"), *(orientation.GetColumn(0)).ToString());
	/*
		new_forward = normalize(velocity);
		approximate_up = normalize(approximate_up); // if needed
		new_side = cross(new_forward, approximate_up);
		new_up = cross(new_forward, new_side);
	*/
}

void AVehicule::seek(AActor *target) {
	FVector desired_velocity = (target->GetActorLocation() - this->GetActorLocation()).GetSafeNormal() * max_speed;
	FVector steering = desired_velocity - velocity;

	calculateNewPosition(steering);
	caculateNewOrientation();

	this->SetActorLocation(position);
	float theta = UKismetMathLibrary::Acos(orientation.GetColumn(0).X);
	UE_LOG(LogTemp, Warning, TEXT("angle : %f"), theta * 180 / PI);
	this->SetActorRotation(FRotator(0, theta * 180 / PI, 0));
}



