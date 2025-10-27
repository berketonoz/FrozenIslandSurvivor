// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InventoryCharacter.generated.h"

UCLASS()
class INVPLUGIN_API AInventoryCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AInventoryCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual FVector GetCameraLocation() { return FVector(); }
	virtual FRotator GetCameraRotation() { return FRotator(); }
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;
};
