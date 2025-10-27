#include "SharedCrafting.h"
#include "../Components/CraftingComponent.h"
#include "Net/UnrealNetwork.h"

ASharedCrafting::ASharedCrafting(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
    bReplicates = true;
    CraftingComponent = CreateDefaultSubobject<UCraftingComponent>(TEXT("CraftingComponent"));
    CraftingComponent->SetIsReplicated(true);
    CraftingComponent->Inventory = StorageComponent;
}

void ASharedCrafting::BeginPlay()
{
    Super::BeginPlay();
}

void ASharedCrafting::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ASharedCrafting, CraftingComponent);
}