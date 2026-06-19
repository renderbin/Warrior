// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abilities/GameplayAbility.h"
#include "CoreMinimal.h"
#include "WarriorGameplayAbility.generated.h"

class UPawnCombatComponet;
class UWarriorAbilitySystemComponent;

UENUM(BlueprintType)
enum class EWarriorAbilityActivationPolity : uint8
{
	OnTriggered,
	OnGiven
};
/**
 *
 */
UCLASS()
class WARRIOR_API UWarriorGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
protected:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AbilitySystem")
	EWarriorAbilityActivationPolity ActivationPolity = EWarriorAbilityActivationPolity::OnTriggered;

	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	UPawnCombatComponet* GetPawnCombatComponetFromActorInfo() const;

	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	UWarriorAbilitySystemComponent* GetWarriorAbilitySystemComponentFromActorInfo() const;
};
