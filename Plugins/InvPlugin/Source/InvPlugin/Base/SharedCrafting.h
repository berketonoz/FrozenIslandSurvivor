#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SharedStorage.h"
#include "../Base/RecipeItem.h"

#include "SharedCrafting.generated.h"

UCLASS()
class INVPLUGIN_API ASharedCrafting : public ASharedStorage
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

public:
    ASharedCrafting(const FObjectInitializer &ObjectInitializer);
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = Inventory)
    class UCraftingComponent *CraftingComponent;
};