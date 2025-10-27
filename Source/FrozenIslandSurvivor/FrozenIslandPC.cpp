// Fill out your copyright notice in the Description page of Project Settings.


#include "FrozenIslandPC.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerInput.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"

void AFrozenIslandPC::SetupInputComponent()
{
    Super::SetupInputComponent();

    // InputComponent->BindAxis("MoveForward", this, &AFrozenIslandPC::MoveForward);
    // InputComponent->BindAxis("MoveRight", this, &AFrozenIslandPC::MoveRight);
    // InputComponent->BindAxis("Turn", this, &AFrozenIslandPC::Turn);
    // InputComponent->BindAxis("LookUp", this, &AFrozenIslandPC::LookUp);
    // InputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AFrozenIslandPC::Jump);
    // InputComponent->BindAction("Jump", ETriggerEvent::Completed, this, &AFrozenIslandPC::StopJumping);
}

void AFrozenIslandPC::BeginPlay()
{
    Super::BeginPlay();
    // Additional initialization if needed
}

void AFrozenIslandPC::PlayerTick(float DeltaTime)
{
    Super::PlayerTick(DeltaTime);
    // Additional per-frame logic if needed
}

// Additional methods or properties can be added here as needed
// This class can be extended with more functionality as required for the game
// For example, handling inventory, crafting, or other game-specific mechanics