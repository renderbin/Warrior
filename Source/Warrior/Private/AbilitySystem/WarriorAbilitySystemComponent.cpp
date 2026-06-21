// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
void UWarriorAbilitySystemComponent::OnAbilityInputPressed(const FGameplayTag& InInputTag)
{
	if (!InInputTag.IsValid())
	{
		return;
	}
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InInputTag))
		{
			continue;
		}
		TryActivateAbility(AbilitySpec.Handle);
	}
}
void UWarriorAbilitySystemComponent::OnAbilityInputReleased(const FGameplayTag& InInputTag)
{
}
void UWarriorAbilitySystemComponent::GrantHeroWeaponAbilities(const TArray<FWarriorHeroAbilitySet>& InDefaultWeaponAbilitySets, int32 ApplyLevel, TArray<FGameplayAbilitySpecHandle>& outGrantedAbilitySpecHandles)
{
	if (InDefaultWeaponAbilitySets.IsEmpty())
	{
		return;
	}
	for (const FWarriorHeroAbilitySet& AbilitySet : InDefaultWeaponAbilitySets)
	{
		if (!AbilitySet.IsValid())
			continue;
		FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
		AbilitySpec.SourceObject = GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilitySet.InputTag);

		outGrantedAbilitySpecHandles.AddUnique(GiveAbility(AbilitySpec));
	}
}
void UWarriorAbilitySystemComponent::RemoveGrantedHeroWeaponAbilities(UPARAM(ref) TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove)
{
	if (InSpecHandlesToRemove.IsEmpty())
	{
		return;
	}
	for (const FGameplayAbilitySpecHandle& SpecHandle : InSpecHandlesToRemove)
	{
		if (SpecHandle.IsValid())
		{
			ClearAbility(SpecHandle);
		}
	}
	InSpecHandlesToRemove.Empty();
}