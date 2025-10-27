// Fill out your copyright notice in the Description page of Project Settings.

#include "ThrowableItem.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/Engine.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"

AThrowableItem::AThrowableItem()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	bReplicates = true;
	// SetReplicates(true);

	// MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// MeshComponent->SetSimulatePhysics(false);
	// SetActorEnableCollision(false);

	// BounceCount = 0;
	// MeshComponent->OnComponentHit.AddDynamic(this, &AThrowableItem::OnItemHit);
}

void AThrowableItem::BeginPlay()
{
	Super::BeginPlay();

	MeshComponent->OnComponentHit.AddDynamic(this, &AThrowableItem::OnItemHit);
}

AController *AThrowableItem::GetOwnerController() const
{
	APawn *OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
		return nullptr;
	return OwnerPawn->GetController();
}

void AThrowableItem::Throw(const FVector &ThrowDirection)
{
	if (ThrowSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ThrowSound, GetActorLocation());
	}
	MeshComponent->AddImpulse(ThrowDirection);

	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetSimulatePhysics(true);
	// SetActorEnableCollision(true);
}

void AThrowableItem::OnItemHit_Implementation(UPrimitiveComponent *HitComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit)
{
	if (HasAuthority())
	{

		if (!OtherActor || OtherActor == this)
			return;

		BounceCount++;

		// Build damage event
		FPointDamageEvent DamageEvent;
		DamageEvent.Damage = Damage;
		DamageEvent.HitInfo = Hit;
		DamageEvent.ShotDirection = (Hit.TraceStart - Hit.TraceEnd).GetSafeNormal();
		DamageEvent.DamageTypeClass = UDamageType::StaticClass();

		if (HitSound && BounceCount < 3)
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *OtherActor->GetName());
			UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
		}
		// Apply damage
		OtherActor->TakeDamage(Damage, DamageEvent, GetOwnerController(), this);
	}
}
