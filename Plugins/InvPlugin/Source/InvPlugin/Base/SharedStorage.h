#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Components/StorageComponent.h"
#include "Placeable.h"

#include "SharedStorage.generated.h"

UCLASS()
class INVPLUGIN_API ASharedStorage : public APlaceable
{
    GENERATED_BODY()

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Sets default values for this actor's properties
    ASharedStorage(const FObjectInitializer &ObjectInitializer);
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = Inventory)
    class UStorageComponent *StorageComponent;

    UFUNCTION(BlueprintCallable, Server, Reliable, Category = Inventory)
    void ServerAddBPItem(FInventoryItem item, int index);

    UFUNCTION(BlueprintCallable, Server, Reliable, Category = Inventory)
    void ServerRemoveBPItem(FInventoryItem item);

    // UFUNCTION(BlueprintCallable, Server, Reliable, Category = Inventory)
    // void ServerRemoveBPItemStack(FName uniqueName, int stackSize);
};