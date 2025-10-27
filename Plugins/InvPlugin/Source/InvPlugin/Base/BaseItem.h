#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Placeable.h"       // Fully define APlaceable
#include "PickupItem.h"      // Fully define APickupItem
#include "BaseItem.generated.h"


UENUM(BlueprintType)
enum eItemType
{
    Unset UMETA(DisplayName = "Unset"),
    Weapon UMETA(DisplayName = "Weapon"),
    Spell UMETA(DisplayName = "Spell"),
    Food UMETA(DisplayName = "Food"),
    Potion UMETA(DisplayName = "Potion"),
    Material UMETA(DisplayName = "Material"),
};

USTRUCT(BlueprintType, Category = Inventory)
struct FItemRow : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
    class UStaticMesh *StaticMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
    class USkeletalMesh *SkeletalMesh;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
    TSubclassOf<class APlaceable> BuildingTemplate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
    FName UniqueName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
    FText ItemDisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory, meta = (MultiLine = true))
    FText ItemDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
    FText UseActionText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
    class UTexture2D *Thumbnail;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
    TSubclassOf<class APickupItem> ItemActorClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
    TEnumAsByte<eItemType> ItemType = eItemType::Unset;
    

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory, meta = (ClampMin = 0.0f))
    float Weight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory, meta = (ExposeOnSpawn = true, ClampMin = 1))
    int StackSize = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory, meta = (ClampMin = 1))
    int MaxStackSize = 100;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory, meta = (ClampMin = 0.0))
    float Cost;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory, meta = (ClampMin = 0.0))
    float SellCost;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
    bool IsShopItem = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
    FString CostType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
    bool IsConsumable = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
    bool IsStackable = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
    bool HasMaxStackSize = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
    bool IsRare = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
    bool IsBound = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
    bool IsEnabled = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
    bool IsAltEnabled = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
    bool HasCooldown = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory, meta = (ClampMin = 0.0))
    float CooldownTime = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
    bool HasAltCooldown = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory, meta = (ClampMin = 0.0))
    float AltCooldownTime = 5.0f;
};
