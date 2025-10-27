// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "../Base/InventoryItem.h"
#include "InventoryPC.generated.h"

/**
 *
 */
UCLASS()
class INVPLUGIN_API AInventoryPC : public APlayerController
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = Inventory)
	void ServerAddBPItem(class UStorageComponent *storage, FInventoryItem item, int index);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = Inventory)
	void ServerRemoveBPItem(class UStorageComponent *storage, FInventoryItem item);
	
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = Inventory)
	void ServerSetTimerBPItem(class UStorageComponent *storage, FName uniqueName, float cooldownTime);
	
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = Inventory)
	void ServerStartCrafting(class UStorageComponent *storage, class UCraftingComponent *crafting, FName uniqueName);
};
