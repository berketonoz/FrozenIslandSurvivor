// Fill out your copyright notice in the Description page of Project Settings.

#include "BuildComponent.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/DataTable.h"
#include "StorageComponent.h"
#include "../Base/InventoryItem.h"
#include "../Base/RecipeItem.h"
#include "../Base/Placeable.h"
#include "../Controllers/InventoryCharacter.h"
#include "Net/UnrealNetwork.h"

UBuildComponent::UBuildComponent()
{

    SetIsReplicatedByDefault(true);
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bTickEvenWhenPaused = false;
    PrimaryComponentTick.TickGroup = TG_PrePhysics;
    SetComponentTickEnabled(true);

    isBuilding = false;
    isActorSpawned = false;
}

void UBuildComponent::BeginPlay()
{
    Super::BeginPlay();

    parent = Cast<AInventoryCharacter>(GetOwner());
    UE_LOG(LogTemp, Warning, TEXT("Parent: %s"), parent ? *parent->GetName() : TEXT("None"));
    if (parent)
    {
        inventory = parent->GetComponentByClass<UStorageComponent>();
    }
}

void UBuildComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UBuildComponent, isBuilding);
    DOREPLIFETIME(UBuildComponent, currentBuilding);
    DOREPLIFETIME(UBuildComponent, isActorSpawned);
    // DOREPLIFETIME(UBuildComponent, isActorPlaceable);
}

void UBuildComponent::OnRep_IsActorPlaceable()
{
    // This function is called on clients when isActorPlaceable is updated on the server.
    // You can add any client-side logic here if needed.
    UE_LOG(LogTemp, Warning, TEXT("isActorPlaceable replicated: %s"), isActorPlaceable ? TEXT("Can Place") : TEXT("Cannot Place"));
}

void UBuildComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (isBuilding)
    {
        ProcessBuildLocation();

        if (currentBuilding && isActorSpawned)
        {
            isActorPlaceable = IsPlacementValid(currentBuilding);
            currentBuilding->SetPlacementValid(isActorPlaceable);
        }
    }
}


void UBuildComponent::ProcessBuildLocation_Implementation()
{
    FVector Location = GetNextBuildLocation();
    UpdateActorPosition(Location);
}

bool UBuildComponent::IsPlacementValid(APlaceable *Building)
{
    if (!Building)
        return false;

    TArray<AActor *> IgnoredActors;
    IgnoredActors.Add(parent);
    IgnoredActors.Add(Building);

    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

    FHitResult OutHit;
    FVector Origin;
    FVector Extent;
    Building->GetActorBounds(true, Origin, Extent);

    bool bHit = UKismetSystemLibrary::BoxTraceSingleForObjects(
        GetWorld(),
        Origin,
        Origin + FVector(0.1f, 0.1f, 0.1f), // tiny offset to make trace valid
        Extent,
        FRotator::ZeroRotator,
        ObjectTypes,
        false,
        IgnoredActors,
        EDrawDebugTrace::None,
        OutHit,
        true);

    return !bHit;
}

bool UBuildComponent::GetHitLoc(FVector Start, FVector End, TArray<AActor *> IgnoreList, FHitResult &outHit)
{
    TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;
    TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
    TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
    TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

    bool result = UKismetSystemLibrary::LineTraceSingleForObjects(
        GetWorld(),
        Start,
        End,
        TraceObjectTypes,
        false,
        IgnoreList,
        EDrawDebugTrace::None,
        OUT outHit,
        true);

    // DrawDebugSphere(GetWorld(), outHit.Location, 10.0f, 12, FColor::Red, false, 1.0f);

    return result;
}

FVector UBuildComponent::GetNextBuildLocation()
{
    return FindLookAtRotation();
}

FRotator UBuildComponent::GetNextBuildRotation()
{
    return currentBuildRotation;
}

FVector UBuildComponent::FindLookAtRotation()
{
    TArray<AActor *> IgnoreList = TArray<AActor *>();
    IgnoreList.Add(parent);
    IgnoreList.Add(currentBuilding);

    FVector HeadPos = parent->GetCameraLocation();
    FRotator PlayerRotation = parent->GetCameraRotation();

    FVector End = HeadPos + (PlayerRotation.Vector() * placeDistance);

    FHitResult outHit;
    bool hasHit = GetHitLoc(HeadPos, End, IgnoreList, OUT outHit);
    if (hasHit)
    {
        return outHit.Location;
    }

    return FVector();
}

void UBuildComponent::ToggleBuildMode_Implementation(bool building, TSubclassOf<APlaceable> Template)
{
    SetBuilding(building, Template);

    if (isBuilding)
    {
        FVector Location = GetNextBuildLocation();
        FRotator Rotation = FRotator(0.0f, 0.0f, 0.0f);
        SpawnActor(Location, Rotation);
    }
    else
    {
        ClearBuild();
    }
}

void UBuildComponent::RequestBuild_Implementation()
{
    if (parent->HasAuthority())
    {
        if (isBuilding && isActorSpawned)
        {
            if (currentBuilding->definition.Recipe.Num() > 0)
            {
                bool canBuild = true;

                for (FInventoryItem &a : currentBuilding->definition.Recipe)
                {
                    if (!inventory->hasItem(a.UniqueName, a.StackSize))
                    {
                        canBuild = false;
                    }
                }

                if (!canBuild)
                    return;

                for (FInventoryItem &a : currentBuilding->definition.Recipe)
                {
                    inventory->RemoveItemStack(a.UniqueName, a.StackSize);
                }
                currentBuilding->Build();
                isBuilding = false;
                isActorSpawned = false;
                currentBuilding = nullptr;
            }
            else
            {
                currentBuilding->Build();
                isBuilding = false;
                isActorSpawned = false;
                currentBuilding = nullptr;
            }
        }
    }
}

void UBuildComponent::RotateBuilding_Implementation(float axis_value)
{
    if (isBuilding)
    {
        currentBuildRotation.Yaw += axis_value * RotSpeed; // Adjust rotation speed as needed
    }
}

void UBuildComponent::ClearBuild_Implementation()
{
    if (parent->HasAuthority())
    {
        if (isActorSpawned)
        {
            isBuilding = false;
            isActorSpawned = false;
            currentBuilding->Destroy();
            currentBuilding = nullptr;
        }
    }
}

void UBuildComponent::SetBuilding_Implementation(bool building, TSubclassOf<APlaceable> Template)
{
    if (parent->HasAuthority())
    {
        isBuilding = building;
        BuildingTemplate = Template;
    }
}

void UBuildComponent::SpawnActor_Implementation(FVector Location, FRotator Rotation)
{
    if (BuildingTemplate)
    {
        FActorSpawnParameters params;
        params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        params.Owner = GetOwner();
        params.Instigator = Cast<APawn>(params.Owner);

        currentBuilding = GetWorld()->SpawnActor<APlaceable>(BuildingTemplate, Location, Rotation, params);
        isActorSpawned = currentBuilding != nullptr;

        if (isActorSpawned)
        {
            // FRecipeItem *item = dataTable->FindRow<FRecipeItem>(currentBuilding->UniqueName, "");
            // if (item)
            // {
            //     currentBuilding->definition = *item;
            // }
            // else
            // {
            //     ClearBuild();
            // }
        }
    }
}

void UBuildComponent::UpdateActorPosition_Implementation(FVector Location)
{
    if (isActorSpawned)
    {

        bool grid = isGrid;
        if (grid)
        {
            FVector v = Location.GridSnap(10);
            currentBuilding->SetActorLocationAndRotation(FVector(v.X, v.Y, Location.Z), currentBuildRotation);
        }
        else
        {
            currentBuilding->SetActorLocationAndRotation(Location, currentBuildRotation);
        }
    }
}
