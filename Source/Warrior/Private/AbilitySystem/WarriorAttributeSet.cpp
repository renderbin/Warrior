// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/WarriorAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGameplayTags.h"
#include "Interfaces/PawnUIInterface.h"
#include "Components/PawnUIComponent.h"
#include "Components/UI/HeroUIComponent.h"
UWarriorAttributeSet::UWarriorAttributeSet() {
  InitCurrentHealth(1.0f);
  InitMaxHealth(1.0f);
  InitCurrentRage(1.0f);
  InitMaxRage(1.0f);
  InitAttackPower(1.0f);
  InitDefensePower(1.0f);
}
void UWarriorAttributeSet::PostGameplayEffectExecute(
    const FGameplayEffectModCallbackData &Data) {

  if (!CachedPawnUIInterface.IsValid()) {
    CachedPawnUIInterface =
        TWeakInterfacePtr<IPawnUIInterface>(Data.Target.GetAvatarActor());
  }

  checkf(CachedPawnUIInterface.IsValid(),
         TEXT("%s didn't implement IPawnUIInterface"),
         *Data.Target.GetAvatarActor()->GetName());
  UPawnUIComponent *PawnUIComponent =
      CachedPawnUIInterface->GetPawnUIComponent();
  checkf(PawnUIComponent, TEXT("Couldn't extract a PawnUIComponent from %s"),
         *Data.Target.GetAvatarActor()->GetName());

  if (Data.EvaluatedData.Attribute == GetCurrentHealthAttribute()) {
    const float NewCurrentHealth =
        FMath::Clamp(GetCurrentHealth(), 0.f, GetMaxHealth());
    SetCurrentHealth(NewCurrentHealth);
    PawnUIComponent->OnCurrentHealthChanged.Broadcast(GetCurrentHealth() /
                                                      GetMaxHealth());
  }


  if (Data.EvaluatedData.Attribute == GetCurrentRageAttribute()) {
    const float NewCurrentRage =
        FMath::Clamp(GetCurrentRage(), 0.f, GetMaxRage());
    SetCurrentRage(NewCurrentRage);
    if (UHeroUIComponent *HeroUIComponent =
            CachedPawnUIInterface->GetHeroUIComponent()) {
      HeroUIComponent->OnCurrentRageChanged.Broadcast(GetCurrentRage() /
                                                      GetMaxRage());
    }
    }

  if (Data.EvaluatedData.Attribute == GetDamageTakenAttribute()) {
    const float OldHealth = GetCurrentHealth();
    const float DamageDone = GetDamageTaken();

    const float NewCurrentHealth =
        FMath::Clamp(OldHealth - DamageDone, 0.f, GetMaxHealth());
    SetCurrentHealth(NewCurrentHealth);
    PawnUIComponent->OnCurrentHealthChanged.Broadcast(GetCurrentHealth() /
                                                      GetMaxHealth());
    if (NewCurrentHealth <= 0.f) {
      UWarriorFunctionLibrary::AddGameplayTagActorIfNone(
          Data.Target.GetAvatarActor(),
          WarriorGameplayTags::Shared_Status_Dead);
    }
  }
}