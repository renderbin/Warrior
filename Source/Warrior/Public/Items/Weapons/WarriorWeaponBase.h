// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WarriorWeaponBase.generated.h"
class UBoxComponent;
class UStaticMeshComponent;

DECLARE_DELEGATE_OneParam(FOnTargetInteractedDelegate, AActor *);
UCLASS()
class WARRIOR_API AWarriorWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWarriorWeaponBase();
        FORCEINLINE UBoxComponent *GetWeaponCollisionBox() const {
          return WeaponCollisionBox;
        }

        FOnTargetInteractedDelegate OnWeaponHitTarget;
        FOnTargetInteractedDelegate OnWeaponPulledFromTarget;

      protected:
        UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
        UStaticMeshComponent *WeaponMesh;

        UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
        UBoxComponent *WeaponCollisionBox;

      protected:
   UFUNCTION()
 virtual void OnCollisionBoxBeginOverlap(
            UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
            UPrimitiveComponent *OtherComp, int32 OtherBodyIndex,
            bool bFromSweep, const FHitResult &SweepResult);

   UFUNCTION()
virtual void OnCollisionBoxEndOverlap(
            UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
            UPrimitiveComponent *OtherComp, int32 OtherBodyIndex);
};
