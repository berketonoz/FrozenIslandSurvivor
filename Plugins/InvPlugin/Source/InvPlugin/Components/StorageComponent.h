// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Delegates/DelegateCombinations.h"
#include "../Base/InventoryItem.h"
#include "StorageComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStorageCompUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStorageCompFull);

USTRUCT(BlueprintType, Category = Inventory)
struct FItemTimer
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	FName UniqueName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	FTimerHandle handle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	float RemainingTime;

	bool operator==(const FInventoryItem &rhs) const { return UniqueName == rhs.UniqueName; }
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class INVPLUGIN_API UStorageComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Sets default values for this component's properties
	UStorageComponent();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

#pragma region Variables

	UPROPERTY(EditDefaultsOnly, Replicated, Category = Inventory)
	int Capacity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = Inventory)
	int SlotsFilled;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_StorageUpdated, Category = Inventory)
	TArray<FInventoryItem> Items;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	class UDataTable *dataTable;

	UPROPERTY(BlueprintAssignable, Category = Inventory)
	FOnStorageCompUpdated OnInventoryUpdated;

	UPROPERTY(BlueprintAssignable, Category = Inventory)
	FOnStorageCompFull OnInventoryFull;

#pragma endregion Variables

#pragma region Functions

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

	UFUNCTION()
	virtual void OnRep_StorageUpdated();

	void UpdateUI();

	int GetFirstEmpty();
	int AddEmptyAtIndex(int index);

	bool AddItem(FInventoryItem item, int index);
	bool RemoveItem(FInventoryItem item);
	bool RemoveItemStack(FName uniqueName, int stackSize);
	bool hasItem(FName uniqueName, int stackSize);

#pragma endregion Functions

#pragma region BP Functions

	UFUNCTION(BlueprintCallable, Category = Inventory)
	TArray<FInventoryItem> GetItems() { return Items; };

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = Inventory)
	void ServerAddBPItem(FInventoryItem item, int index);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = Inventory)
	void ServerRemoveBPItem(FInventoryItem item);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = Inventory)
	void ServerRemoveBPItemStack(FName uniqueName, int stackSize);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool BPHasItem(FName uniqueName, int stackSize);

#pragma endregion BP Functions

#pragma region Cooldown

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_OnTimersUpdated, Category = Inventory)
	TArray<FItemTimer> ItemTimers;

	UPROPERTY(BlueprintAssignable, Category = Inventory)
	FOnStorageCompUpdated OnTimersUpdated;

	UFUNCTION()
	virtual void OnRep_OnTimersUpdated();

	void SetTimerForItem(FName uniqueName, float cooldownTime);
	void ClearTimerForItem(FName uniqueName);
	float getTimeRemainingForItem(FName uniqueName);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = Inventory)
	void ServerBPSetTimerForItem(FName uniqueName, float cooldownTime);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = Inventory)
	void ServerBPClearTimerForItem(FName uniqueName);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	float BPgetTimeRemainingForItem(FName uniqueName);

#pragma endregion Cooldown
};
