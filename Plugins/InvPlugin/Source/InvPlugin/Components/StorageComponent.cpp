// Fill out your copyright notice in the Description page of Project Settings.

#include "StorageComponent.h"
#include "Engine/DataTable.h"
#include "../Base/BaseItem.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

UStorageComponent::UStorageComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	ItemTimers = TArray<FItemTimer>();
}

void UStorageComponent::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < Capacity; i++)
	{
		FInventoryItem item = FInventoryItem();
		item.index = i;
		item.StackSize = 0;
		item.ItemOwner = this;
		item.isEmpty = true;
		Items.Add(item);
	}
}

void UStorageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetOwner()->HasAuthority())
	{
		for (FItemTimer &timer : ItemTimers)
		{
			if (GetWorld()->GetTimerManager().IsTimerActive(timer.handle))
			{
				timer.RemainingTime = GetWorld()->GetTimerManager().GetTimerRemaining(timer.handle);
			}
		}
	}
}

void UStorageComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UStorageComponent, Capacity);
	DOREPLIFETIME(UStorageComponent, SlotsFilled);
	DOREPLIFETIME(UStorageComponent, ItemTimers);
	DOREPLIFETIME(UStorageComponent, Items);
}

void UStorageComponent::OnRep_StorageUpdated()
{
	OnInventoryUpdated.Broadcast();
}

void UStorageComponent::UpdateUI()
{
	OnInventoryUpdated.Broadcast();
}

int UStorageComponent::GetFirstEmpty()
{
	for (FInventoryItem &a : Items)
	{
		if (a.isEmpty)
		{
			return a.index;
		}
	}
	return -1;
}

int UStorageComponent::AddEmptyAtIndex(int index)
{
	FInventoryItem e = FInventoryItem();
	e.index = index;
	e.isEmpty = true;
	e.ItemOwner = this;
	Items.Insert(e, index);
	UpdateUI();
	return index;
}

bool UStorageComponent::AddItem(FInventoryItem item, int index)
{

	if (!dataTable)
		return false;

	const FItemRow *Row = dataTable->FindRow<FItemRow>(item.UniqueName, TEXT("AddItem"));
	if (!Row)
	{
		UE_LOG(LogTemp, Warning, TEXT("Item not found in DataTable: %s"), *item.UniqueName.ToString());
		return false;
	}

	if (SlotsFilled >= Capacity)
	{
		// Inventory is full, broadcast the event
		OnInventoryFull.Broadcast();
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("Adding item: %s"), *item.UniqueName.ToString());
	if (dataTable)
	{
		FItemRow *ItemRow = dataTable->FindRow<FItemRow>(item.UniqueName, "");
		int pendingStackSize = item.StackSize;
		UE_LOG(LogTemp, Log, TEXT("ItemRow found: %s, StackSize: %d"), *item.UniqueName.ToString(), pendingStackSize);

		if (ItemRow->IsStackable)
		{
			for (FInventoryItem &a : Items)
			{
				if (a == item)
				{
					if (!ItemRow->HasMaxStackSize)
					{
						a.StackSize += pendingStackSize;
						UE_LOG(LogTemp, Log, TEXT("Item already exists in inventory, updated stack size: %s"), *a.UniqueName.ToString());
						UpdateUI();
						return true;
					}
					else
					{
						if (a.StackSize < ItemRow->MaxStackSize)
						{
							int oldStackSize = a.StackSize;

							if ((oldStackSize + pendingStackSize) > ItemRow->MaxStackSize)
							{
								int diff = (oldStackSize + pendingStackSize) - ItemRow->MaxStackSize;
								a.StackSize = ItemRow->MaxStackSize;
								pendingStackSize = diff;
							}
							else
							{
								a.StackSize += pendingStackSize;
								pendingStackSize = 0;
							}
						}
					}
				}

				if (pendingStackSize == 0)
				{
					UE_LOG(LogTemp, Log, TEXT("Item already exists in inventory, updated stack size: %s"), *a.UniqueName.ToString());
					UpdateUI();
					return true;
				}
			}
		}

		UE_LOG(LogTemp, Log, TEXT("Item is not stackable or no existing item found, adding as new item: %s"), *item.UniqueName.ToString());

		int newIndex = -1;
		if (index == -1)
			newIndex = GetFirstEmpty();
		else
			newIndex = index;
		Items.RemoveAt(newIndex);

		item.index = newIndex;
		item.StackSize = pendingStackSize; // Assuming we are adding a new item with stack size of 1
		item.ItemOwner = this;			   // Set the owner of the item

		item.isEnabled = (getTimeRemainingForItem(item.UniqueName) == 0);

		Items.Insert(item, newIndex);
		SlotsFilled++;

		UE_LOG(LogTemp, Log, TEXT("Item added at index %d: %s"), newIndex, *item.UniqueName.ToString());
		// Update UI
		UpdateUI();
		UE_LOG(LogTemp, Log, TEXT("Item added successfully: %s"), *item.UniqueName.ToString());
		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("DataTable not set or item not found in DataTable: %s"), *item.UniqueName.ToString());

	return false;
}

void UStorageComponent::ServerAddBPItem_Implementation(FInventoryItem item, int index)
{
	AddItem(item, index);
}

bool UStorageComponent::RemoveItem(FInventoryItem item)
{
	int index = item.index;
	if (item.index > -1)
	{
		Items.RemoveAt(index);
		AddEmptyAtIndex(index);
		SlotsFilled--;
		UpdateUI();
		return true;
	}
	return false;
}

bool UStorageComponent::hasItem(FName uniqueName, int stackSize)
{
	int pendingCount = stackSize;

	for (FInventoryItem &a : Items)
	{
		if (a.UniqueName == uniqueName)
		{
			pendingCount -= a.StackSize;
			if (pendingCount <= 0)
			{
				return true;
			}
		}
	}
	return false;
}

bool UStorageComponent::RemoveItemStack(FName uniqueName, int stackSize)
{
	int pendingCount = stackSize;

	for (FInventoryItem &a : Items)
	{
		if (a.UniqueName == uniqueName)
		{
			if (a.StackSize <= pendingCount)
			{
				pendingCount -= a.StackSize;
				RemoveItem(a);
			}
			else
			{
				a.StackSize -= pendingCount;
				pendingCount = 0;
			}
		}

		if (pendingCount <= 0)
		{
			UpdateUI();
			return true;
		}
	}

	return false;
}

void UStorageComponent::ServerRemoveBPItem_Implementation(FInventoryItem item)
{
	RemoveItem(item);
}

bool UStorageComponent::BPHasItem(FName uniqueName, int stackSize)
{
	return hasItem(uniqueName, stackSize);
}

void UStorageComponent::ServerRemoveBPItemStack_Implementation(FName uniqueName, int stackSize)
{
	RemoveItemStack(uniqueName, stackSize);
}

#pragma region Cooldown

void UStorageComponent::OnRep_OnTimersUpdated()
{
	OnTimersUpdated.Broadcast();
}

void UStorageComponent::SetTimerForItem(FName uniqueName, float cooldownTime)
{
	if (uniqueName.GetStringLength() == 0)
	{
		return;
	}

	for (FInventoryItem &item : Items)
	{
		if (!item.isEmpty && item.UniqueName == uniqueName)
		{
			item.isEnabled = false;
		}
	}

	bool timerExists = false;
	for (FItemTimer t : ItemTimers)
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
		FItemTimer t = FItemTimer();
		t.UniqueName = uniqueName;
		t.handle = FTimerHandle();
		new_index = ItemTimers.Add(t);
	}

	if (new_index > -1)
	{
		FItemTimer &t = ItemTimers[new_index];

		FTimerDelegate ClearDelegate = FTimerDelegate::CreateUObject(
			this, &UStorageComponent::ClearTimerForItem, uniqueName);
		GetWorld()->GetTimerManager().SetTimer(t.handle, ClearDelegate, cooldownTime, false);
		t.RemainingTime = cooldownTime;
	}
}

void UStorageComponent::ClearTimerForItem(FName uniqueName)
{
	bool timerExists = false;
	int index = 0;

	for (int i = 0; i < ItemTimers.Num(); i++)
	{
		if (ItemTimers[i].UniqueName == uniqueName)
		{
			index = i;
			timerExists = true;
			break;
		}
	}

	if (timerExists)
	{
		FItemTimer &t = ItemTimers[index];
		GetWorld()->GetTimerManager().ClearTimer(t.handle);
		ItemTimers.RemoveAt(index);
	}

	for (FInventoryItem &item : Items)
	{
		if (!item.isEmpty && item.UniqueName == uniqueName)
		{
			item.isEnabled = true;
		}
	}
}

float UStorageComponent::getTimeRemainingForItem(FName uniqueName)
{
	bool timerExists = false;
	int index = 0;

	for (int i = 0; i < ItemTimers.Num(); i++)
	{
		if (ItemTimers[i].UniqueName == uniqueName)
		{
			index = i;
			timerExists = true;
			break;
		}
	}

	if (timerExists)
	{
		FItemTimer &t = ItemTimers[index];
		return t.RemainingTime;
		// return GetWorld()->GetTimerManager().GetTimerRemaining(t.handle);
	}
	return 0;
}

void UStorageComponent::ServerBPSetTimerForItem_Implementation(FName uniqueName, float cooldownTime)
{
	SetTimerForItem(uniqueName, cooldownTime);
}

void UStorageComponent::ServerBPClearTimerForItem_Implementation(FName uniqueName)
{
	ClearTimerForItem(uniqueName);
}

float UStorageComponent::BPgetTimeRemainingForItem(FName uniqueName)
{
	return getTimeRemainingForItem(uniqueName);
}

#pragma endregion Cooldown