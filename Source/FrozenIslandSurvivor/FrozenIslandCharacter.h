// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <InvPlugin/Controllers/InventoryCharacter.h>
#include "ThrowableItem.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "Enums/E_Item.h"
#include "InputActionValue.h"

#include "FrozenIslandCharacter.generated.h"

struct FInputActionValue;

// Declaration of the delegate that will be called when the Primary Action is triggered
// It is declared as dynamic so it can be accessed also in Blueprints
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUseItem);

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS()
class FROZENISLANDSURVIVOR_API AFrozenIslandCharacter : public AInventoryCharacter
{
	GENERATED_BODY()

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext *DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction *JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction *MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction *LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction *PrimaryAction;

public:
	AFrozenIslandCharacter();

	/** Health & State */
	UFUNCTION(BlueprintPure)
	bool IsDead() const;

	UFUNCTION(BlueprintPure)
	float GetHealthPercent() const;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController *EventInstigator, AActor *DamageCauser) override;

	UFUNCTION(Server, Reliable)
	void ServerTakeDamage(float DamageAmount, FDamageEvent const &DamageEvent, AController *EventInstigator, AActor *DamageCauser);

	class UCameraComponent *CamRef;
	virtual FVector GetCameraLocation();
	virtual FRotator GetCameraRotation();

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnUseItem OnUseItem;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyControllerChanged() override;
	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;

	void OnPrimaryActionPressed();
	void OnPrimaryActionReleased();

	// virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

private:
	/** Movement Input */
	void Move(const FInputActionValue &Value);
	void Look(const FInputActionValue &Value);

	UFUNCTION(Server, Reliable)
	void SwitchWeapon(FKey Key);

	// UFUNCTION(Server, Reliable)
	void SpawnThrowableItem();

	// UFUNCTION(Server, Reliable)
	// void ServerSetCurrentAimRotation();

	UFUNCTION(Server, Reliable)
	void ServerAim();
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastAim();
	
	void Aim();
	
	UFUNCTION(Server, Reliable)
	void ServerRelease();
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRelease();

	void Release();

	// UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Throw", meta = (AllowPrivateAccess = "true"))
	// FRotator CurrentAimRotation;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
	E_Item CurrentItem;

	UPROPERTY(EditDefaultsOnly, Category = "Throw")
	TSubclassOf<AActor> ThrowableItemClass;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Throw", meta = (AllowPrivateAccess = "true"))
	AThrowableItem *CurrentThrowableItem;

	/** Components */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	class UStorageComponent *StorageComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	class UBuildComponent *BuildComponent;

	/** Movement Settings */
	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float MaxHealth = 100;

	UPROPERTY(ReplicatedUsing=OnRep_Health, VisibleAnywhere, Category = "Stats")
	float Health;

	UFUNCTION()
	void OnRep_Health();

	UPROPERTY(EditAnywhere, Category = "Stats")
	float RotationRate = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float MoveSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float TurnRateGamepad = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float LookUpRateGamepad = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throw", meta = (AllowPrivateAccess = "true"))
	UAnimMontage *AimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throw", meta = (AllowPrivateAccess = "true"))
	UAnimMontage *ThrowMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Throw", meta = (AllowPrivateAccess = "true"))
	bool bIsChargingThrow = false;
};
