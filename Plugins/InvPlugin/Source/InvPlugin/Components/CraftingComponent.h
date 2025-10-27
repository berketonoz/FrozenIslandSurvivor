// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Delegates/DelegateCombinations.h"
#include "StorageComponent.h"
#include "../Base/RecipeItem.h"
#include "CraftingComponent.generated.h"

USTRUCT(BlueprintType, Category = Inventory)
struct FRecipeTimer
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	FName UniqueName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	FTimerHandle handle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	float CraftStartTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	float CraftDuration;

	bool operator==(const FRecipeItem &rhs) const { return UniqueName == rhs.UniqueName; }
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class INVPLUGIN_API UCraftingComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Sets default values for this component's properties
	UCraftingComponent();
	// virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

#pragma region Variables

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = Crafting)
	bool isCrafting;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = Crafting)
	class UStorageComponent *Inventory;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = Crafting)
	class UDataTable *dataTable;

#pragma endregion Variables

#pragma region BP Functions

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = Inventory)
	void StartCrafting(FName uniqueName);

	UFUNCTION()
	void ProcessCrafting(FName uniqueName);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = Inventory)
	void SetIsCrafting(bool is_crafting);

#pragma endregion BP Functions

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	TArray<FRecipeItem> Recipes;

#pragma region Cooldown

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_OnTimersUpdated, Category = Inventory)
	TArray<FRecipeTimer> RecipeTimers;

	UPROPERTY(BlueprintAssignable, Category = Inventory)
	FOnStorageCompUpdated OnTimersUpdated;

	UFUNCTION()
	virtual void OnRep_OnTimersUpdated();

	void SetTimerForRecipe(FName uniqueName, float cooldownTime);
	void ClearTimerForRecipe(FName uniqueName);
	float getTimeRemainingForRecipe(FName uniqueName);
	void CancelCrafting(FName uniqueName);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	float BPgetTimeRemainingForRecipe(FName uniqueName);

#pragma endregion Cooldown
};
