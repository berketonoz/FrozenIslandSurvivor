// Fill out your copyright notice in the Description page of Project Settings.

#include "FrozenIslandCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <InvPlugin/Components/StorageComponent.h>
#include <InvPlugin/Components/BuildComponent.h>
#include "DrawDebugHelpers.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AFrozenIslandCharacter::AFrozenIslandCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	StorageComponent = CreateDefaultSubobject<UStorageComponent>(TEXT("StorageComponent"));
	BuildComponent = CreateDefaultSubobject<UBuildComponent>(TEXT("BuildComponent"));

	SetReplicates(true);
	SetReplicateMovement(true);
}

void AFrozenIslandCharacter::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
	CurrentItem = E_Item::Unarmed;

	CamRef = GetComponentByClass<UCameraComponent>();

	// if (AController* OwningController = GetController())
	// {
	// 	SetOwner(OwningController);
	// }
}

void AFrozenIslandCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// IsActorPlaceable UE_LOG
	// UE_LOG(LogTemp, Warning, TEXT("IsActorPlaceable: %s"), BuildComponent->isActorPlaceable ? TEXT("Can Place") : TEXT("Cannot Place"));
}

FVector AFrozenIslandCharacter::GetCameraLocation()
{
	return CamRef->GetComponentLocation();
}

FRotator AFrozenIslandCharacter::GetCameraRotation()
{
	return CamRef->GetComponentRotation();
}

void AFrozenIslandCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController *PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem *Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AFrozenIslandCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent *EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFrozenIslandCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFrozenIslandCharacter::Look);

		// Primary Action
		EnhancedInputComponent->BindAction(PrimaryAction, ETriggerEvent::Started, this, &AFrozenIslandCharacter::OnPrimaryActionPressed);
		EnhancedInputComponent->BindAction(PrimaryAction, ETriggerEvent::Completed, this, &AFrozenIslandCharacter::OnPrimaryActionReleased);
	}

	// Switch Weapon
	PlayerInputComponent->BindKey(EKeys::One, IE_Pressed, this, &AFrozenIslandCharacter::SwitchWeapon);
	PlayerInputComponent->BindKey(EKeys::Two, IE_Pressed, this, &AFrozenIslandCharacter::SwitchWeapon);
}

void AFrozenIslandCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate health
	DOREPLIFETIME(AFrozenIslandCharacter, Health);
	DOREPLIFETIME(AFrozenIslandCharacter, CurrentItem);
	DOREPLIFETIME(AFrozenIslandCharacter, CurrentThrowableItem);
	DOREPLIFETIME(AFrozenIslandCharacter, bIsChargingThrow);
	// DOREPLIFETIME(AFrozenIslandCharacter, CurrentAimRotation);
}

void AFrozenIslandCharacter::OnRep_Health()
{
	// This function is called on clients when Health is updated
	// You can add code here to update UI or play effects based on health changes
	UE_LOG(LogTemp, Warning, TEXT("Health updated: %f"), Health);
}

// ---------------------------------------------------------------------------------------------- //
// ////////////////////////////////////////// INPUT ///////////////////////////////////////////// //
// ---------------------------------------------------------------------------------------------- //
#pragma region Input

void AFrozenIslandCharacter::Move(const FInputActionValue &Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller)
	{
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AFrozenIslandCharacter::Look(const FInputActionValue &Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);

		// ServerSetCurrentAimRotation();
	}
}

void AFrozenIslandCharacter::SwitchWeapon_Implementation(FKey Key)
{
	if (Key == EKeys::One)
	{
		CurrentItem = E_Item::Unarmed;
	}
	else if (Key == EKeys::Two)
	{
		CurrentItem = E_Item::Throwable;

		SpawnThrowableItem();
		// ServerEquipThrowableItem();
	}
}

// void AFrozenIslandCharacter::ServerSetCurrentAimRotation_Implementation()
// {
// 	CurrentAimRotation = GetControlRotation();
// }

void AFrozenIslandCharacter::OnPrimaryActionPressed()
{
	if (CurrentItem == E_Item::Throwable && !BuildComponent->isBuilding)
	{
		ServerAim();
	}
}

void AFrozenIslandCharacter::OnPrimaryActionReleased()
{
	if (bIsChargingThrow)
	{
		ServerRelease();
		return;
	}

	if (BuildComponent->isBuilding)
	{
		// if (BuildComponent->isActorPlaceable)
		BuildComponent->RequestBuild();
	}
	else
	{
		OnUseItem.Broadcast();
	}
}

#pragma endregion Input

// -------------------------
// Health & State
// -------------------------

bool AFrozenIslandCharacter::IsDead() const
{
	return Health <= 0;
}

float AFrozenIslandCharacter::GetHealthPercent() const
{
	return Health / MaxHealth;
}

float AFrozenIslandCharacter::TakeDamage(float DamageAmount, FDamageEvent const &DamageEvent, AController *EventInstigator, AActor *DamageCauser)
{

	float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (HasAuthority())
	{
		DamageToApply = FMath::Min(Health, DamageToApply);
		Health -= DamageToApply;
		UE_LOG(LogTemp, Warning, TEXT("SERVER: Took Damage: %f"), DamageToApply);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CLIENT: Took Damage: %f"), DamageToApply);
	}
	return DamageToApply;
}

void AFrozenIslandCharacter::ServerTakeDamage_Implementation(float DamageAmount, FDamageEvent const &DamageEvent, AController *EventInstigator, AActor *DamageCauser)
{
	TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AFrozenIslandCharacter::ServerAim_Implementation()
{
	MulticastAim();
}

void AFrozenIslandCharacter::MulticastAim_Implementation()
{
	Aim();
}

void AFrozenIslandCharacter::Aim()
{
	if (CurrentItem == E_Item::Throwable)
	{
		bIsChargingThrow = true;
		if (UAnimInstance *AnimInstance = GetMesh()->GetAnimInstance())
		{
			AnimInstance->Montage_Play(AimMontage);
			if (HasAuthority())
			{
				UE_LOG(LogTemp, Warning, TEXT("SERVER: Playing Aim Montage"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("CLIENT: Playing Aim Montage"));
			}
		}
	}
}

void AFrozenIslandCharacter::ServerRelease_Implementation()
{
	MulticastRelease();
}

void AFrozenIslandCharacter::MulticastRelease_Implementation()
{
	Release();
}

void AFrozenIslandCharacter::Release()
{
	if (bIsChargingThrow)
	{
		bIsChargingThrow = false;
		// PlayAnimMontage(ThrowMontage);
		if (UAnimInstance *AnimInstance = GetMesh()->GetAnimInstance())
		{
			AnimInstance->Montage_Play(ThrowMontage);

			if (CurrentThrowableItem)
			{
				CurrentThrowableItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				CurrentThrowableItem->GetMesh()->SetSimulatePhysics(true);
				CurrentThrowableItem->GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

				// FVector ThrowDirectionVector = CurrentAimRotation.Vector() * 2000.0f;
				FVector ThrowDirectionVector = GetControlRotation().Vector() * 2000.0f;

				CurrentThrowableItem->Throw(ThrowDirectionVector);

				if (CurrentItem == E_Item::Throwable)
				{
					SpawnThrowableItem();
					if (HasAuthority())
					{
						UE_LOG(LogTemp, Warning, TEXT("SERVER: Released Throwable Item"));
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("CLIENT: Released Throwable Item"));
					}
				}
			}
		}
	}
}

void AFrozenIslandCharacter::SpawnThrowableItem()
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.Instigator = GetInstigator();
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	CurrentThrowableItem = GetWorld()->SpawnActor<AThrowableItem>(ThrowableItemClass, SpawnParameters);

	auto *Mesh = CurrentThrowableItem->GetMesh();
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Attach on SERVER (attachment replicates)
	CurrentThrowableItem->AttachToComponent(
		GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		TEXT("hand_rSocket"));
}
