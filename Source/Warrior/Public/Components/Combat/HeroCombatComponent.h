// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Combat/PawnCombatComponet.h"
#include "HeroCombatComponent.generated.h"

class AWarriorHeroWeapon;
/**
 * 
 */
UCLASS()
class WARRIOR_API UHeroCombatComponent : public UPawnCombatComponet
{
	GENERATED_BODY()
	public:
		UFUNCTION(BlueprintCallable, Category="Warrior|Combat")
		AWarriorHeroWeapon* GetHeroCharacterCarriedWeaponByTag(FGameplayTag InWeaponTag) const;
	
	
	
};
