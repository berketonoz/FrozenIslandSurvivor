// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryPC.h"
#include "../Components/StorageComponent.h"
#include "../Components/CraftingComponent.h"

void AInventoryPC::ServerStartCrafting_Implementation(UStorageComponent *storage, UCraftingComponent *crafting, FName uniqueName)
{
    crafting->Inventory = storage;
    // crafting->UniqueName = uniqueName;
    crafting->StartCrafting(uniqueName);
}

void AInventoryPC::ServerAddBPItem_Implementation(UStorageComponent *storage, FInventoryItem item, int index)
{
    storage->AddItem(item, index);
}

void AInventoryPC::ServerRemoveBPItem_Implementation(UStorageComponent *storage, FInventoryItem item)
{
    storage->RemoveItem(item);
}

void AInventoryPC::ServerSetTimerBPItem_Implementation(UStorageComponent *storage, FName uniqueName, float cooldownTime)
{
    storage->SetTimerForItem(uniqueName, cooldownTime);
}