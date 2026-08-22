// Fill out your copyright notice in the Description page of Project Settings.

#include "DataAssets/Input/DataAsset_InputConfig.h"
#include "InputAction.h"
#include "InputMappingContext.h"
UInputAction *UDataAsset_InputConfig::FindNativeActionByTag(
    const FGameplayTag &InInputTag) const {
  for (const FWarriorInputActionConfig &InputActionConfig :
       NativeInputActions) {
    if (InputActionConfig.InputTag == InInputTag &&
        IsValid(InputActionConfig.InputAction)) {
      return InputActionConfig.InputAction;
    }
  }
  return nullptr;
}
