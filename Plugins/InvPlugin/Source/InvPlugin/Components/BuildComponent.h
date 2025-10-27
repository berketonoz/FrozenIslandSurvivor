// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Delegates/DelegateCombinations.h"
#include "Engine/HitResult.h"

#include "BuildComponent.generated.h"

// Forward declarations for faster compile and minimal coupling
class APlaceable;
class AInventoryCharacter;
class UStorageComponent;
class UCameraComponent;
class UDataTable;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class INVPLUGIN_API UBuildComponent : public UActorComponent
{
    GENERATED_BODY()

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

public:
    // Sets default values for this component's properties
    UBuildComponent();
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

    UFUNCTION() 
    void OnRep_IsActorPlaceable();

#pragma region Functions

    // UFUNCTION(BlueprintCallable, Client, Reliable, Category = "Building")
    UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Building")
    void ProcessBuildLocation();

    bool IsPlacementValid(APlaceable *Building);

    bool GetHitLoc(FVector Start, FVector End, TArray<AActor *> IgnoreList, FHitResult &outHit);

    FVector GetNextBuildLocation();
    FRotator GetNextBuildRotation();
    FVector FindLookAtRotation();
    // FVector SnapToGrid(FVector Location) const;

    UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Building")
    void ToggleBuildMode(bool building, TSubclassOf<class APlaceable> Template);

    UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Building")
    void SetBuilding(bool building, TSubclassOf<class APlaceable> Template);

    UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Building")
    void SpawnActor(FVector Location, FRotator Rotation);

    UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Building")
    void RequestBuild();

    UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Building")
    void ClearBuild();

    UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Building")
    void RotateBuilding(float axis_value);

    UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Building")
    void UpdateActorPosition(FVector Location);

    
    #pragma endregion Functions
    
    #pragma region Variables
    
    UPROPERTY(EditAnywhere, Category = "Building")
    bool isGrid;
    
    class UCameraComponent *playerCamera;
    class UStorageComponent *inventory;
    class AInventoryCharacter *parent;
    
    FRotator currentBuildRotation;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Building)
    APlaceable *currentBuilding;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Building)
    bool isActorSpawned;
    
    // UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Building)
    // bool isActorPlaceable;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_IsActorPlaceable, Category = Building)
    bool isActorPlaceable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Building)
    TSubclassOf<class APlaceable> BuildingTemplate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Building)
    class UDataTable *dataTable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Building)
    bool isBuilding = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Building)
    float RotSpeed = 45.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Building)
    float buildDistance = 45.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Building)
    float placeDistance = 1000.0f;

#pragma endregion Variables
};
