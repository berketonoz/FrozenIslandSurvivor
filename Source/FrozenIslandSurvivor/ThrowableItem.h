// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ThrowableItem.generated.h"

UCLASS()
class FROZENISLANDSURVIVOR_API AThrowableItem : public AActor
{
	GENERATED_BODY()

public:
	AThrowableItem();

	virtual void BeginPlay() override;
	
	// UFUNCTION(BlueprintCallable)
	void Throw(const FVector &ThrowDirection);

	// UFUNCTION(BlueprintCallable)
	UStaticMeshComponent *GetMesh() const { return MeshComponent; }

protected:
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void OnItemHit(UPrimitiveComponent *HitComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit);

	UPROPERTY(EditDefaultsOnly)
	float Damage = 10.0f;

private:
	AController *GetOwnerController() const;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent *MeshComponent;

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase *ThrowSound;

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase *HitSound;

	AActor *PreviousActor;
	int BounceCount = 0;


};
