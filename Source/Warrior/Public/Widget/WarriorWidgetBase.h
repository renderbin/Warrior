// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WarriorWidgetBase.generated.h"
class UHeroUIComponent;
class UEnemyUIComponent;
/**
 *
 */
UCLASS()
class WARRIOR_API UWarriorWidgetBase : public UUserWidget {
  GENERATED_BODY()

public:
  UFUNCTION(BlueprintCallable)
  void InitEnemyCreatedWidget(AActor *OwningEnemyActor);

protected:
  virtual void NativeOnInitialized() override;

  // 蓝图可实现事件，在成功获得 HeroUIComponent 后调用
  UFUNCTION(BlueprintImplementableEvent,
            meta = (DisplayName = "On Owning Hero UI Component Initialized"))
  void BP_OnOwningHeroUIComponentInitialized(
      UHeroUIComponent *OwningHeroUIComponent);

  // 蓝图实现事件：初始化完成后，把 enemy UI component 交给蓝图去绑定委托
  UFUNCTION(BlueprintImplementableEvent,
            meta = (DisplayName = "On Owning Enemy UI Component Initialized"))
  void BP_OnOwningEnemyUIComponentInitialized(
      UEnemyUIComponent *OwningEnemyUIComponent);
};
