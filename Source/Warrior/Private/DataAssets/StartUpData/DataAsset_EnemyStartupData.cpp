// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/StartUpData/DataAsset_EnemyStartupData.h"
#include "AbilitySystem/Abilities/WarriorEnemyGameplayAbility.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
void UDataAsset_EnemyStartupData::GiveToAbilitySystemComponent(
    UWarriorAbilitySystemComponent *InASCToGive, int32 ApplyLevel) {
  Super::GiveToAbilitySystemComponent(InASCToGive, ApplyLevel);
  if (EnemyCombatAbilities.IsEmpty())
    return;
  for (const TSubclassOf<UWarriorEnemyGameplayAbility> &AbilityClass :
       EnemyCombatAbilities) {
    if (!AbilityClass)
      continue;
    FGameplayAbilitySpec AbilitySpec(AbilityClass);
    AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
    AbilitySpec.Level = ApplyLevel;
    InASCToGive->GiveAbility(AbilitySpec);
  }
}
