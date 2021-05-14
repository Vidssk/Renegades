// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HandControls/HandControllerBase.h"
#include "VRCharacter.generated.h"

UCLASS()
class RENEGADES_API AVRCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVRCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
private:
	void FBMove(float AxisValue);
	void LRMove(float AxisValue);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetupHands();
	void UpdateRootLocation();
private:

	//Configuration
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AHandControllerBase> RightHandControllerClass;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AHandControllerBase> LeftHandControllerClass;

	//Components
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* VRRoot;
	
	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* Camera;

	UPROPERTY()
	AHandControllerBase* LeftHandController;
	UPROPERTY()
	AHandControllerBase* RightHandController;

};
