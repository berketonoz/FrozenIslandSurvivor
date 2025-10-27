#include "PickupItem.h"
#include "../Components/StorageComponent.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

APickupItem::APickupItem(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
    bReplicates = true;
}

void APickupItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(APickupItem, UniqueName);
    DOREPLIFETIME(APickupItem, StackSize);
}

void APickupItem::BeginPlay()
{
    Super::BeginPlay();
}

void APickupItem::PickupItem_Implementation(ACharacter *p)
{
    TArray<UStorageComponent *> comps;
    UStorageComponent *storageComp;
    if (p)
    {
        p->GetComponents<UStorageComponent>(comps);
        storageComp = p->FindComponentByClass<UStorageComponent>();

        if (comps.Num() > 0)
        {
            UStorageComponent *first = comps[0];

            UE_LOG(LogTemp, Warning, TEXT("Adding to component: %s"), *first->GetName());

            if (first != nullptr)
            {
                FInventoryItem item = FInventoryItem();
                item.UniqueName = UniqueName;
                item.StackSize = StackSize;
                item.ItemOwner = first;
                first->AddItem(item, -1);
                Destroy();
            }
        }
    }
}