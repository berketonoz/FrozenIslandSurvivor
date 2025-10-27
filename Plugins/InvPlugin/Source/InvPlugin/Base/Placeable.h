#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "RecipeItem.h"
#include "Placeable.generated.h"

UCLASS()
class INVPLUGIN_API APlaceable : public APawn
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

public:
    APlaceable(const FObjectInitializer &ObjectInitializer);

    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Placeable")
    UStaticMeshComponent *StaticBuildMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Placeable")
    class UBoxComponent *CollisionVolume;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Placeable")
    FName UniqueName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Placeable")
    FRecipeItem definition;

    UPROPERTY(EditDefaultsOnly, Category = "Building")
    UMaterialInterface *ValidMaterial;

    UPROPERTY(EditDefaultsOnly, Category = "Building")
    UMaterialInterface *InvalidMaterial;

    UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "Placeable")
    void Build();

    UFUNCTION(BlueprintCallable, Category = "Building")
    void SetPlacementValid(bool bIsValid);
};