// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Controllers/WarriorHeroController.h"
#include "WarriorGameplayTags.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Components/Combat/HeroCombatComponent.h"
AWarriorHeroCharacter *
UWarriorHeroGameplayAbility::GetHeroCharacterFromActorInfo() {
  if (!CachedWarriorHeroCharacter.IsValid()) {
    CachedWarriorHeroCharacter =
        Cast<AWarriorHeroCharacter>(CurrentActorInfo->AvatarActor);
  }
  return CachedWarriorHeroCharacter.IsValid() ? CachedWarriorHeroCharacter.Get()
                                              : nullptr;
}

AWarriorHeroController *
UWarriorHeroGameplayAbility::GetHeroControllerFromActorInfo() {
  if (!CachedWarriorHeroController.IsValid()) {
    CachedWarriorHeroController =
        Cast<AWarriorHeroController>(CurrentActorInfo->PlayerController);
  }
  return CachedWarriorHeroController.IsValid()
             ? CachedWarriorHeroController.Get()
             : nullptr;
}

UHeroCombatComponent *
UWarriorHeroGameplayAbility::GetHeroCombatComponentFromActorInfo() {
  return GetHeroCharacterFromActorInfo()->GetHeroCombatComponent();
}

FGameplayEffectSpecHandle
UWarriorHeroGameplayAbility::MakeHeroDamageEffectSpecHandle (
    TSubclassOf<UGameplayEffect> EffectClass, float InWeaponBaseDamage,
    FGameplayTag InCurrentAttackTypeTag, int32 InUsedComboCount) const {
  ensure(EffectClass);
  const UWarriorAbilitySystemComponent *ASC =
      GetWarriorAbilitySystemComponentFromActorInfo();
  AActor *AvatarActor = GetAvatarActorFromActorInfo();

  FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
  ContextHandle.SetAbility(this);
  ContextHandle.AddSourceObject(AvatarActor);
  ContextHandle.AddInstigator(AvatarActor, AvatarActor);

  FGameplayEffectSpecHandle EffectSpecHandle =
      ASC->MakeOutgoingSpec(EffectClass, GetAbilityLevel(), ContextHandle);
  if (EffectSpecHandle.IsValid()) {
    EffectSpecHandle.Data->SetSetByCallerMagnitude(
        WarriorGameplayTags::Shared_SetByCaller_BaseDamage, InWeaponBaseDamage);
    if (InCurrentAttackTypeTag.IsValid()) {
      EffectSpecHandle.Data->SetSetByCallerMagnitude(InCurrentAttackTypeTag,
                                                     InUsedComboCount);
    }
  }
  return EffectSpecHandle;
}