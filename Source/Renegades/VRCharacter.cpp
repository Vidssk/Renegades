// Fill out your copyright notice in the Description page of Project Settings.


#include "VRCharacter.h"

#include "Camera/CameraComponent.h"

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
