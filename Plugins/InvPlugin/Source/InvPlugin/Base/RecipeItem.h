#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "InventoryItem.h"
#include "RecipeItem.generated.h"

USTRUCT(BlueprintType, Category = Inventory)
struct FRecipeItem: public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
    FName UniqueName;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
    class UTexture2D *Thumbnail;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
    FString ItemDisplayName;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
    FString ItemDescription;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
    bool canChain = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
    int LevelOrSkill = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
    int OutputStackSize = 1;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
    float BuildTime = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
    TArray<FInventoryItem> Recipe;

    bool operator==(const FRecipeItem &rhs) const { return UniqueName == rhs.UniqueName; }
};
