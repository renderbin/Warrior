// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "WarriorTypes/WarriorEnumTypes.h"
#include "WarriorGameplayAbility.generated.h"

class UPawnCombatComponent;
class UWarriorAbilitySystemComponent;
UENUM(BlueprintType)
enum class EWarriorAbilityActivationPolicy : uint8 { OnTriggered, OnGiven };
/**
 *
 */
UCLASS()
class WARRIOR_API UWarriorGameplayAbility : public UGameplayAbility {
  GENERATED_BODY()

protected:
  virtual void OnGiveAbility(const FGameplayAbilityActorInfo *ActorInfo,
                             const FGameplayAbilitySpec &Spec) override;

  virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
                          const FGameplayAbilityActorInfo *ActorInfo,
                          const FGameplayAbilityActivationInfo ActivationInfo,
                          bool bReplicateEndAbility,
                          bool bWasCancelled) override;

  // 激活策略变量，可在蓝图中编辑
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AbilitySystem")
  EWarriorAbilityActivationPolicy ActivationPolicy =
      EWarriorAbilityActivationPolicy::OnTriggered;

protected:
  UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
  UPawnCombatComponent *GetPawnCombatComponentFromActorInfo() const;

  UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
  UWarriorAbilitySystemComponent *
  GetWarriorAbilitySystemComponentFromActorInfo() const;

  FActiveGameplayEffectHandle
  ApplyEffectSpecHandleToTarget(AActor *TargetActor,
                                const FGameplayEffectSpecHandle &InSpecHandle);
  UFUNCTION(BlueprintCallable,
            meta = (DisplayName = "Apply Effect SpecHandle to Target",
                    ExpandEnumAsExecs = "OutSuccessType"))
  FActiveGameplayEffectHandle BP_ApplyEffectSpecHandleToTarget(
      AActor *TargetActor, const FGameplayEffectSpecHandle &InSpecHandle,
      EWarriorSuccessType &OutSuccessType);
};
