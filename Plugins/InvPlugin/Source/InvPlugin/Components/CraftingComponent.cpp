// Fill out your copyright notice in the Description page of Project Settings.

#include "CraftingComponent.h"

// Engine/core
#include "Engine/World.h"            // For GetWorld()
#include "TimerManager.h"            // For FTimerDelegate, FTimerHandle
#include "Net/UnrealNetwork.h"
#include "Engine/DataTable.h"

// Project-specific
#include "../Base/InventoryItem.h"
#include "../Base/RecipeItem.h"


UCraftingComponent::UCraftingComponent()
{

    SetIsReplicatedByDefault(true);

    RecipeTimers = TArray<FRecipeTimer>();
}

void UCraftingComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UCraftingComponent, isCrafting);
    DOREPLIFETIME(UCraftingComponent, Inventory);
    DOREPLIFETIME(UCraftingComponent, RecipeTimers);
}

void UCraftingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // if (GetOwner()->HasAuthority())
    // {
    //     for (FRecipeTimer &timer : RecipeTimers)
    //     {
    //         if (GetWorld()->GetTimerManager().IsTimerActive(timer.handle))
    //         {
    //             // timer.RemainingTime = GetWorld()->GetTimerManager().GetTimerRemaining(timer.handle);
    //             // UE_LOG(LogTemp, Log, TEXT("Timer for %s: %f seconds remaining"), *timer.UniqueName.ToString(), timer.RemainingTime);
    //         }
    //     }
    // }
}

void UCraftingComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UCraftingComponent::StartCrafting_Implementation(FName uniqueName)
{
    isCrafting = !isCrafting;

    if (isCrafting)
    {
        UE_LOG(LogTemp, Log, TEXT("Starting crafting for: %s"), *uniqueName.ToString());
        if (uniqueName.GetStringLength() > 0 && dataTable)
        {
            // Get Recipe
            FRecipeItem *Item = dataTable->FindRow<FRecipeItem>(uniqueName, TEXT(""));

            if (Inventory)
            {
                // Proccess whether the player has the ingredients
                bool hasIngredients = true;

                for (FInventoryItem &a : Item->Recipe)
                {
                    if (!Inventory->hasItem(a.UniqueName, a.StackSize))
                    {
                        hasIngredients = false;
                    }
                }

                if (hasIngredients)
                {
                    // Remove said ingredients from the inventory
                    for (FInventoryItem &a : Item->Recipe)
                    {
                        Inventory->RemoveItemStack(a.UniqueName, a.StackSize);
                    }

                    if (Item->BuildTime > 0.0f)
                    {
                        SetTimerForRecipe(uniqueName, Item->BuildTime);
                    }
                }
            }
        }
    }
    else
    {
        // Stop the crafting process
        CancelCrafting(uniqueName);
    }
}

void UCraftingComponent::ProcessCrafting(FName uniqueName)
{
    FRecipeItem *Item = dataTable->FindRow<FRecipeItem>(uniqueName, TEXT(""));
    UE_LOG(LogTemp, Log, TEXT("Processing crafting for: %s"), *uniqueName.ToString());
    if (Item && Inventory)
    {
        FInventoryItem newItem = FInventoryItem();
        newItem.UniqueName = uniqueName;
        newItem.StackSize = Item->OutputStackSize;
        Inventory->AddItem(newItem, -1);
        UE_LOG(LogTemp, Log, TEXT("Crafted item: %s"), *uniqueName.ToString());
    }
}

void UCraftingComponent::SetIsCrafting_Implementation(bool is_crafting)
{
    isCrafting = is_crafting;
}

void UCraftingComponent::CancelCrafting(FName uniqueName)
{
    FRecipeItem *Item = dataTable->FindRow<FRecipeItem>(uniqueName, TEXT(""));
    if (Item && Inventory)
    {
        ClearTimerForRecipe(uniqueName);
        for (FInventoryItem &a : Item->Recipe)
        {
            Inventory->AddItem(a, -1);
        }
    }
}

#pragma region Cooldown

void UCraftingComponent::OnRep_OnTimersUpdated()
{
    OnTimersUpdated.Broadcast();
}

void UCraftingComponent::SetTimerForRecipe(FName uniqueName, float cooldownTime)
{
    if (uniqueName.GetStringLength() == 0)
    {
        return;
    }

    bool timerExists = false;
    for (FRecipeTimer t : RecipeTimers)
    {
        if (t.UniqueName == uniqueName)
        {
            timerExists = true;
            break;
        }
    }

    int new_index = -1;
    if (!timerExists)
    {
        FRecipeTimer t = FRecipeTimer();
        t.UniqueName = uniqueName;
        t.handle = FTimerHandle();
        new_index = RecipeTimers.Add(t);
    }

    if (new_index > -1)
    {
        FRecipeTimer &t = RecipeTimers[new_index];

        FTimerDelegate CraftDelegate = FTimerDelegate::CreateLambda([this, uniqueName]()
                                                                    {
            ProcessCrafting(uniqueName);
            ClearTimerForRecipe(uniqueName);
            isCrafting = !isCrafting; });

        GetWorld()->GetTimerManager().SetTimer(t.handle, CraftDelegate, cooldownTime, false);
        // t.RemainingTime = cooldownTime;
        t.CraftStartTime = GetWorld()->GetTimeSeconds();
        t.CraftDuration = cooldownTime;
    }
}

void UCraftingComponent::ClearTimerForRecipe(FName uniqueName)
{
    bool timerExists = false;
    int index = 0;

    for (int i = 0; i < RecipeTimers.Num(); i++)
    {
        if (RecipeTimers[i].UniqueName == uniqueName)
        {
            index = i;
            timerExists = true;
            break;
        }
    }

    if (timerExists)
    {
        FRecipeTimer &t = RecipeTimers[index];
        GetWorld()->GetTimerManager().ClearTimer(t.handle);
        RecipeTimers.RemoveAt(index);
    }
}

float UCraftingComponent::getTimeRemainingForRecipe(FName uniqueName)
{
    bool timerExists = false;
    int index = 0;

    for (int i = 0; i < RecipeTimers.Num(); i++)
    {
        if (RecipeTimers[i].UniqueName == uniqueName)
        {
            index = i;
            timerExists = true;
            break;
        }
    }

    if (timerExists)
    {
        FRecipeTimer &t = RecipeTimers[index];
        // UE_LOG(LogTemp, Log, TEXT("Time remaining for %s: %f"), *uniqueName.ToString(), t.RemainingTime);
        // return t.RemainingTime;
        return GetWorld()->GetTimerManager().GetTimerRemaining(t.handle);
    }
    return 0;
}

float UCraftingComponent::BPgetTimeRemainingForRecipe(FName uniqueName)
{
    return getTimeRemainingForRecipe(uniqueName);
}

#pragma endregion Cooldown