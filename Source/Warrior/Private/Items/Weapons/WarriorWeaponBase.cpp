// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Weapons/WarriorWeaponBase.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AWarriorWeaponBase::AWarriorWeaponBase() {
  PrimaryActorTick.bCanEverTick = false;
  WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
  SetRootComponent(WeaponMesh);

  WeaponCollisionBox =
      CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollisionBox"));
  WeaponCollisionBox->SetupAttachment(GetRootComponent());
  WeaponCollisionBox->SetBoxExtent(FVector(20.f));
  WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  WeaponCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(
      this, &ThisClass::OnCollisionBoxBeginOverlap);
  WeaponCollisionBox->OnComponentEndOverlap.AddUniqueDynamic(
      this, &ThisClass::OnCollisionBoxEndOverlap);
}
void AWarriorWeaponBase::OnCollisionBoxBeginOverlap(
    UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
    UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep,
    const FHitResult &SweepResult) {
  APawn *WeaponOwningPawn = GetInstigator<APawn>();
  checkf(WeaponOwningPawn,
         TEXT("Forgot to assign an instigator as the owning pawn for the "
              "weapon: %s"),
         *GetName());

  // TODO : Implement hit check for enemy characters
  if (APawn *HitPawn = Cast<APawn>(OtherActor)) {
    OnWeaponHitTarget.ExecuteIfBound(OtherActor);
  }
}

void AWarriorWeaponBase::OnCollisionBoxEndOverlap(
    UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
    UPrimitiveComponent *OtherComp, int32 OtherBodyIndex) {
  APawn *WeaponOwningPawn = GetInstigator<APawn>();
  checkf(WeaponOwningPawn,
         TEXT("Forgot to assign an instigator as the owning pawn for the "
              "weapon: %s"),
         *GetName());

  // TODO : Implement hit check for enemy characters
  if (APawn *HitPawn = Cast<APawn>(OtherActor)) {
    OnWeaponPulledFromTarget.ExecuteIfBound(OtherActor);
  }
}
