// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/WarriorEnemyGameplayAbility.h"
#include "Characters/WarriorEnemyCharacter.h"
#include "Components/Combat/EnemyCombatComponent.h"
AWarriorEnemyCharacter *
UWarriorEnemyGameplayAbility::GetEnemyCharacterFromActorInfo() {
  if (!CachedWarriorEnemyCharacter.IsValid())
    CachedWarriorEnemyCharacter =
        Cast<AWarriorEnemyCharacter>(CurrentActorInfo->AvatarActor);
  return CachedWarriorEnemyCharacter.IsValid()
             ? CachedWarriorEnemyCharacter.Get()
             : nullptr;
}

UEnemyCombatComponent *
UWarriorEnemyGameplayAbility::GetEnemyCombatComponentFromActorInfo() {
  AWarriorEnemyCharacter *EnemyCharacter = GetEnemyCharacterFromActorInfo();
  return EnemyCharacter ? EnemyCharacter->GetEnemyCombatComponent() : nullptr;
}