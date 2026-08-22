// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Components/Combat/PawnCombatComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
void UWarriorGameplayAbility::OnGiveAbility(
    const FGameplayAbilityActorInfo *ActorInfo,
    const FGameplayAbilitySpec &Spec) {
  Super::OnGiveAbility(ActorInfo, Spec);

  // 仅当策略为 OnGiven 且技能未激活时执行
  if (ActivationPolicy == EWarriorAbilityActivationPolicy::OnGiven) {
    if (ActorInfo && !Spec.IsActive()) {
      ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
    }
  }
}
void UWarriorGameplayAbility::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo *ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility, bool bWasCancelled) {
  Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility,
                    bWasCancelled);

  // OnGiven 技能结束后立即从 ASC 移除
  if (ActivationPolicy == EWarriorAbilityActivationPolicy::OnGiven) {
    if (ActorInfo) {
      ActorInfo->AbilitySystemComponent->ClearAbility(Handle);
    }
  }
}

UPawnCombatComponent *
UWarriorGameplayAbility::GetPawnCombatComponentFromActorInfo() const {
  return GetAvatarActorFromActorInfo()
      ->FindComponentByClass<UPawnCombatComponent>();
}

UWarriorAbilitySystemComponent *
UWarriorGameplayAbility::GetWarriorAbilitySystemComponentFromActorInfo() const {
  return Cast<UWarriorAbilitySystemComponent>(
      CurrentActorInfo->AbilitySystemComponent);
}

FActiveGameplayEffectHandle UWarriorGameplayAbility::ApplyEffectSpecHandleToTarget(
    AActor *TargetActor, const FGameplayEffectSpecHandle &InSpecHandle) {
  UAbilitySystemComponent *TargetASC =
      UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
  check(TargetASC);
  check(InSpecHandle.IsValid());
  return GetWarriorAbilitySystemComponentFromActorInfo()
      ->ApplyGameplayEffectSpecToTarget(*InSpecHandle.Data, TargetASC);
}

FActiveGameplayEffectHandle
UWarriorGameplayAbility::BP_ApplyEffectSpecHandleToTarget(
    AActor *TargetActor, const FGameplayEffectSpecHandle &InSpecHandle,
    EWarriorSuccessType &OutSuccessType) {
  const FActiveGameplayEffectHandle ActiveEffectHandle =
      ApplyEffectSpecHandleToTarget(TargetActor, InSpecHandle);
  OutSuccessType = ActiveEffectHandle.WasSuccessfullyApplied()
                       ? EWarriorSuccessType::Successful
                       : EWarriorSuccessType::Failed;
  return ActiveEffectHandle;
}