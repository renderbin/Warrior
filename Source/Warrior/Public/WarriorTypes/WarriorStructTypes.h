// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ScalableFloat.h"
#include "WarriorStructTypes.generated.h"

class UWarriorGameplayAbility;
class UInputMappingContext;
class UWarriorHeroLinkedAnimLayer;

USTRUCT(BlueprintType)
struct FWarriorHeroAbilitySet {
  GENERATED_BODY()

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
            meta = (Categories = "Input"))
  FGameplayTag InputTag;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
  TSubclassOf<UWarriorGameplayAbility> AbilityToGrant;

  bool IsValid() const; // 有效性检查
};

USTRUCT(BlueprintType)
struct FWarriorHeroWeaponData {
  GENERATED_BODY()

  // 用于链接的动画层类
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
  TSubclassOf<UWarriorHeroLinkedAnimLayer> WeaponAnimLayerToLink;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
            meta = (TitleProperty = "InputTag"))
  TArray<FWarriorHeroAbilitySet> DefaultWeaponAbilities;

  // 武器专用输入映射上下文
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
  UInputMappingContext *WeaponInputMappingContext;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
  FScalableFloat WeaponBaseDamage;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
  TSoftObjectPtr<UTexture2D> SoftWeaponIconTexture;
};
