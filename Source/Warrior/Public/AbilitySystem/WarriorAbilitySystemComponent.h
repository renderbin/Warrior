// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "DataAssets/StartUpData/DataAsset_HeroStartUpData.h"
#include "WarriorAbilitySystemComponent.generated.h"

/**
 *
 */
UCLASS()
class WARRIOR_API UWarriorAbilitySystemComponent
    : public UAbilitySystemComponent {
  GENERATED_BODY()

public:
  void OnAbilityInputPressed(const FGameplayTag &InInputTag);
  void OnAbilityInputReleased(const FGameplayTag &InInputTag);
  UFUNCTION(BlueprintCallable, meta = (ApplyLevel = "1"))
  void GrantHeroWeaponAbilities(
      const TArray<FWarriorHeroAbilitySet> &InDefaultWeaponAbilities,
      int32 ApplyLevel,
      TArray<FGameplayAbilitySpecHandle> &OutGrantedAbilitySpecHandles);
  UFUNCTION(BlueprintCallable)
  void RemoveGrantedHeroWeaponAbilities(
      UPARAM(ref) TArray<FGameplayAbilitySpecHandle> &InSpecHandlesToRemove);
};
