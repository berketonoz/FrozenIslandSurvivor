#include "Placeable.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

APlaceable::APlaceable(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
    SetReplicates(true);
    SetReplicateMovement(true);

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    
    CollisionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionVolume"));
    CollisionVolume->SetupAttachment(RootComponent);
    CollisionVolume->SetIsReplicated(true);

    StaticBuildMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    StaticBuildMesh->SetupAttachment(CollisionVolume);
    StaticBuildMesh->SetIsReplicated(true);

}

void APlaceable::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void APlaceable::Build_Implementation()
{
    // rsponsible for placed animations
    
}

void APlaceable::SetPlacementValid(bool bIsValid)
{
    if (UStaticMeshComponent* Mesh = FindComponentByClass<UStaticMeshComponent>())
    {
        if (bIsValid && ValidMaterial)
        {
            Mesh->SetMaterial(0, ValidMaterial);
        }
        else if (!bIsValid && InvalidMaterial)
        {
            Mesh->SetMaterial(0, InvalidMaterial);
        }
    }
}

void APlaceable::BeginPlay()
{
    Super::BeginPlay();
}
