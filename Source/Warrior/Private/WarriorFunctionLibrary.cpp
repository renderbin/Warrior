// Fill out your copyright notice in the Description page of Project Settings.

#include "WarriorFunctionLibrary.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/Combat/PawnCombatComponent.h"
#include "Interfaces/PawnCombatInterface.h"

UWarriorAbilitySystemComponent *
UWarriorFunctionLibrary::NativeGetWarriorASCFromActor(AActor *InActor) {
  check(InActor);

  return CastChecked<UWarriorAbilitySystemComponent>(
      UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InActor));
}

void UWarriorFunctionLibrary::AddGameplayTagActorIfNone(AActor *InActor,
                                                        FGameplayTag TagToAdd) {
  UWarriorAbilitySystemComponent *ASC = NativeGetWarriorASCFromActor(InActor);
  if (!ASC->HasMatchingGameplayTag(TagToAdd)) {
    ASC->AddLooseGameplayTag(TagToAdd);
  }
}

void UWarriorFunctionLibrary::RemoveGameplayTagActorIfFound(
    AActor *InActor, FGameplayTag TagToRemove) {
  UWarriorAbilitySystemComponent *ASC = NativeGetWarriorASCFromActor(InActor);
  if (ASC->HasMatchingGameplayTag(TagToRemove)) {
    ASC->RemoveLooseGameplayTag(TagToRemove);
  }
}

bool UWarriorFunctionLibrary::NativeDoesActorHaveTag(AActor *InActor,
                                                     FGameplayTag TagToCheck) {
  UWarriorAbilitySystemComponent *ASC = NativeGetWarriorASCFromActor(InActor);
  return ASC->HasMatchingGameplayTag(TagToCheck);
}

void UWarriorFunctionLibrary::BP_DoesActorHaveTag(
    AActor *InActor, FGameplayTag TagToCheck,
    EWarriorConfirmType &OutConfirmType) {
  OutConfirmType = NativeDoesActorHaveTag(InActor, TagToCheck)
                       ? EWarriorConfirmType::Yes
                       : EWarriorConfirmType::No;
}

UPawnCombatComponent *
UWarriorFunctionLibrary::NativeGetPawnCombatComponentFromActor(
    AActor *InActor) {
  if (!IsValid(InActor))
    return nullptr;
  IPawnCombatInterface *CombatInterface = Cast<IPawnCombatInterface>(InActor);
  return CombatInterface ? CombatInterface->GetPawnCombatComponent() : nullptr;
}

UPawnCombatComponent *
UWarriorFunctionLibrary::BP_GetPawnCombatComponentFromActor(
    AActor *InActor, EWarriorValidType &OutValidType) {
  UPawnCombatComponent *CombatComponent =
      NativeGetPawnCombatComponentFromActor(InActor);
  OutValidType = IsValid(CombatComponent) ? EWarriorValidType::Valid
                                          : EWarriorValidType::Invalid;
  return CombatComponent;
}