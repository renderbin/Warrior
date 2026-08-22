// Fill out your copyright notice in the Description page of Project Settings.


#include "WarriorTypes/WarriorStructTypes.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "InputMappingContext.h"
#include "AnimInstances/Hero/WarriorHeroLinkedAnimLayer.h"
bool FWarriorHeroAbilitySet::IsValid() const {
  return InputTag.IsValid() && AbilityToGrant;
}