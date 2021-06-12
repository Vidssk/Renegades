// Fill out your copyright notice in the Description page of Project Settings.


#include "VRCharacter.h"

#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathVectorCommon.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AVRCharacter::AVRCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(VRRoot);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(VRRoot);

}

// Called when the game starts or when spawned
void AVRCharacter::BeginPlay()
{
	Super::BeginPlay();
	SetupHands();

	//UE_LOG(LogTemp, Warning, TEXT("End of BeginPlay"));

}

// Called every frame
void AVRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateRootLocation();
	//CalculateMovement();
}

// Called to bind functionality to input
void AVRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{

	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//PlayerInputComponent->BindAxis(TEXT("Move_Y"), this, &AVRCharacter::FBMove);
	//PlayerInputComponent->BindAxis(TEXT("Move_X"), this, &AVRCharacter::LRMove);

}

void AVRCharacter::UpdateRootLocation()
{
	FVector NewCameraOffset = Camera->GetComponentLocation() - GetActorLocation();
	NewCameraOffset.Z = 0;
	AddActorWorldOffset(NewCameraOffset);
	VRRoot->AddWorldOffset(-NewCameraOffset);
}


void AVRCharacter::FBMove(float AxisValue)
{
	UE_LOG(LogTemp, Warning, TEXT("FBMove called"));
	AddMovementInput(Camera->GetForwardVector() * AxisValue);
}

void AVRCharacter::LRMove(float AxisValue)
{
	AddMovementInput(Camera->GetRightVector() * AxisValue);
}

FVector AVRCharacter::RotationForwardVector(FTransform Transform)
{
	return Transform.GetRotation().GetForwardVector();
}


float AVRCharacter::GetDirection(FTransform CurrentPosition)
{
	FVector CurrentRotation, CurrentLocation;
	FVector LastRotation, LastLocation;
	FVector CurrentRotationForwardVector = RotationForwardVector(CurrentPosition);
	FVector LastRotationForwardVector = RotationForwardVector(LastCameraPosition);
	FVector CurrentLocationVector = CurrentPosition.GetLocation();
	FVector LastLocationVector = LastCameraPosition.GetLocation();

	CurrentRotation.X = CurrentRotationForwardVector.X; CurrentRotation.Y = CurrentRotationForwardVector.Y; CurrentRotation.Normalize();
	LastRotation.X = LastRotationForwardVector.X; LastRotation.Y = LastRotationForwardVector.Y; LastRotation.Normalize();

	CurrentLocation.X = CurrentLocationVector.X; CurrentLocation.Y = CurrentLocationVector.Y; CurrentLocation.Normalize();
	LastLocation.X = LastLocationVector.X; LastLocation.Y = LastLocationVector.Y; LastLocation.Normalize();

	float Angle = UKismetMathLibrary::DegAcos(FVector::DotProduct(((LastRotation + CurrentRotation) * .5), (CurrentLocation + LastLocation)));
	FVector CrossProduct = FVector::CrossProduct(((LastRotation + CurrentRotation) * .5), (CurrentLocation - LastLocation));
	if (CrossProduct.Z > 0)
	{
		return Angle * -1;
	}
	else
	{
		return Angle * 1;
	}
	return 0.0f;
}

void AVRCharacter::SetupHands()
{
	if (RightHandControllerClass)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Right Hand Function"));
		RightHandController = GetWorld()->SpawnActor<AHandControllerBase>(RightHandControllerClass);
		RightHandController->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
		RightHandController->SetHand(EControllerHand::Right);
		RightHandController->SetOwner(this);
	}

	if (LeftHandControllerClass)
	{
	//	UE_LOG(LogTemp, Warning, TEXT("Left Hand Function"));
		LeftHandController = GetWorld()->SpawnActor<AHandControllerBase>(LeftHandControllerClass);
		LeftHandController->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
		LeftHandController->SetHand(EControllerHand::Left);
		LeftHandController->SetOwner(this);
	}
}

void AVRCharacter::CalculateMovement()
{

	FTransform CurrentCameraPosition = Camera->GetComponentTransform();
	DistanceMoved = GetDistanceMoved(CurrentCameraPosition.GetLocation().X, CurrentCameraPosition.GetLocation().Y);
	DistanceRotated = GetDistanceRotated(CurrentCameraPosition.Rotator());
	if (DistanceMoved > MovementThreshold || DistanceRotated > RotationThreshold) 
	{
		Offset BodyOffset = GetBodyOffset();
		BodyTargetPosition.SetLocation(BodyOffset.Location);
		//BodyTargetPosition.Rotator() = BodyOffset.Rotation;
		BodyTargetPosition.SetRotation(BodyOffset.Rotation.Quaternion());
		UE_LOG(LogTemp, Warning, TEXT("BodyOffset Rotation: %f"), BodyOffset.Rotation.Yaw);
		MovementDirection = GetDirection(CurrentCameraPosition);
		LastCameraPosition = CurrentCameraPosition;
		MovementSpeed = DistanceMoved / UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
		//UE_LOG(LogTemp, Warning, TEXT("MovementSpeed: %f"), DistanceMoved);
	}
	else if( BodyTargetPosition.Equals(CurrentCameraPosition))
	{
		Alpha = 0; MovementSpeed = 0; MovementDirection = 0;
	}
		Alpha = FMath::FInterpTo(Alpha, 1, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), MovementSpeed);
		UKismetMathLibrary::TLerp(CurrentCameraPosition, BodyTargetPosition, Alpha);
//		UE_LOG(LogTemp, Warning, TEXT("Yaw Rotation: %f"), CurrentCameraPosition.Rotator().Yaw);
		GetMesh()->SetWorldLocationAndRotation(BodyTargetPosition.GetLocation(), BodyTargetPosition.GetRotation());
}

float AVRCharacter::GetDistanceMoved(float CurrentX, float CurrentY)
{
	return FVector::Dist(FVector(CurrentX, CurrentY, 0), LastCameraPosition.GetLocation());
}

float AVRCharacter::GetDistanceRotated(FRotator CurrentRotation)
{
	return abs(CurrentRotation.Yaw - LastCameraPosition.GetLocation().Rotation().Yaw);
}

FRotator AVRCharacter::GetBodyRotationFromHMD()
{
	float CurrentYaw = Camera->GetComponentTransform().Rotator().Yaw;
	return FRotator(0, CurrentYaw - 90, 0);
}

Offset AVRCharacter::GetBodyOffset()
{
	float OffsetYaw = Camera->GetComponentTransform().Rotator().Yaw-90;
	FVector LocationOffset;
	Offset BodyOffset;
	FRotator RotationOffset(0, OffsetYaw, 0);
	FVector CameraLocation = Camera->GetComponentLocation();
	LocationOffset = CameraLocation + (RotationOffset.Vector().RightVector * -20);
	LocationOffset.Z = LocationOffset.Z - PlayerHeight;
	BodyOffset.Location = LocationOffset;
	BodyOffset.Rotation = RotationOffset;
	return BodyOffset;
}