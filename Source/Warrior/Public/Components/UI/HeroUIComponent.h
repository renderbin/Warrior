// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnUIComponent.h"
#include "HeroUIComponent.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnEquippedWeaponChanged,
    const TSoftObjectPtr<UTexture2D>, // 参数类型
    SoftWeaponIcon                    // 参数名称
);
/**
 * 
 */
UCLASS()
class WARRIOR_API UHeroUIComponent : public UPawnUIComponent
{
	GENERATED_BODY()

      public:
        UPROPERTY(BlueprintAssignable)
        FOnPercentChangedDelegate OnCurrentRageChanged;

        UPROPERTY(BlueprintAssignable, BlueprintCallable)
FOnEquippedWeaponChanged OnEquippedWeaponChanged;
};
