// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/Input/DataAsset_InputConfig.h"
#include "EnhancedInputComponent.h"
#include "WarriorInputComponent.generated.h"

/**
 *
 */
UCLASS()
class WARRIOR_API UWarriorInputComponent : public UEnhancedInputComponent {
  GENERATED_BODY()

public:
  /**
   * 模板绑定函数，封装输入动作查找与 BindAction 调用
   * @param InInputConfig  输入配置数据资产（携带动作引用）
   * @param InInputTag     用于查找对应 UInputAction 的 GameplayTag
   * @param TriggerEvent   触发事件类型（Pressed/Released/Held 等）
   * @param ContextObject  绑定上下文对象（通常为 this）
   * @param Func           回调函数（成员函数指针或 Lambda）
   */
  template <typename UserObject, typename CallbackFunc>
  void BindNativeInputAction(const UDataAsset_InputConfig *InInputConfig,
                             const FGameplayTag &InInputTag,
                             ETriggerEvent TriggerEvent,
                             UserObject *ContextObject, CallbackFunc Func);

  template <class UserObject, typename CallbackFunc>
  void BindAbilityInputAction(const UDataAsset_InputConfig *InInputConfig,
                              UserObject *ContextObject,
                              CallbackFunc InputPressedFunc,
                              CallbackFunc InputReleasedFunc);
};
template <class UserObject, typename CallbackFunc>
inline void UWarriorInputComponent::BindNativeInputAction(
    const UDataAsset_InputConfig *InInputConfig, const FGameplayTag &InInputTag,
    ETriggerEvent TriggerEvent, UserObject *ContextObject, CallbackFunc Func) {
  checkf(InInputConfig,
         TEXT("Input config data asset is null, can not proceed with binding"));

  if (UInputAction *FoundAction =
          InInputConfig->FindNativeActionByTag(InInputTag)) {
    BindAction(FoundAction, TriggerEvent, ContextObject, Func);
  }
}

template <class UserObject, typename CallbackFunc>
inline void UWarriorInputComponent::BindAbilityInputAction(
    const UDataAsset_InputConfig *InInputConfig, UserObject *ContextObject,
    CallbackFunc InputPressedFunc, CallbackFunc InputReleasedFunc) {
  checkf(InInputConfig,
         TEXT("Input config data asset is null,can not proceed with binding"));
  for (const FWarriorInputActionConfig &AbilityInputActionConfig :
       InInputConfig->AbilityInputActions) {
    if (!AbilityInputActionConfig.IsValid())
      continue;
    BindAction(AbilityInputActionConfig.InputAction, ETriggerEvent::Started,
               ContextObject, InputPressedFunc,
               AbilityInputActionConfig.InputTag);
    BindAction(AbilityInputActionConfig.InputAction, ETriggerEvent::Completed,
               ContextObject, InputReleasedFunc,
               AbilityInputActionConfig.InputTag);
  }
}
