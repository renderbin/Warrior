// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Weapons/WarriorWeaponBase.h"
#include "WarriorTypes/WarriorStructTypes.h"
#include "GameplayAbilitySpecHandle.h"
#include "WarriorHeroWeapon.generated.h"

/**
 * 
 */
UCLASS()
class WARRIOR_API AWarriorHeroWeapon : public AWarriorWeaponBase
{
	GENERATED_BODY()

      public:
        // 武器数据，包含动画层等
        UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponData")
        FWarriorHeroWeaponData HeroWeaponData;

        UFUNCTION(BlueprintCallable, Category = "Warrior|AbilitySystem")
        void AssignGrantedAbilitySpecHandles(
            const TArray<FGameplayAbilitySpecHandle> &InSpecHandles);

        UFUNCTION(BlueprintPure, Category = "Warrior|AbilitySystem")
        TArray<FGameplayAbilitySpecHandle> GetGrantedAbilitySpecHandles() const;

      private:
        TArray<FGameplayAbilitySpecHandle> GrantedAbilitySpecHandles;
};
