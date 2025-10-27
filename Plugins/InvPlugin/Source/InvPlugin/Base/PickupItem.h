#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Components/StorageComponent.h"

#include "PickupItem.generated.h"

UCLASS()
class INVPLUGIN_API APickupItem : public AActor
{
    GENERATED_BODY()

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Sets default values for this actor's properties
    APickupItem(const FObjectInitializer &ObjectInitializer);
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Inventory, meta = (exposeOnSpawn = "true"))
    FName UniqueName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Inventory, meta = (exposeOnSpawn = "true"))
    int StackSize = 1;

    UFUNCTION(BlueprintCallable, Server, Reliable, Category = Inventory)
    void PickupItem(ACharacter *p);

    UFUNCTION(BlueprintImplementableEvent, Category = Inventory)
    void OnHovered();
};