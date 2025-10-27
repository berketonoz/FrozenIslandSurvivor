// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include <InvPlugin/Controllers/InventoryPC.h>
#include "FrozenIslandPC.generated.h"

/**
 *
 */
UCLASS()
class FROZENISLANDSURVIVOR_API AFrozenIslandPC : public AInventoryPC
{
	GENERATED_BODY()
	
protected:
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
	virtual void PlayerTick(float DeltaTime) override;

};
