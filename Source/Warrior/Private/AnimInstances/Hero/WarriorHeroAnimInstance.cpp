// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstances/Hero/WarriorHeroAnimInstance.h"
#include "Characters/WarriorHeroCharacter.h"

void UWarriorHeroAnimInstance::NativeInitializeAnimation() {
  Super::NativeInitializeAnimation();

  if (OwningCharacter) {
    OwningHeroCharacter = Cast<AWarriorHeroCharacter>(OwningCharacter);
  }
}
void UWarriorHeroAnimInstance::NativeUpdateAnimation(float DeltaSeconds) {
  Super::NativeUpdateAnimation(DeltaSeconds);

  // 根据是否有加速度判断角色是否在移动
  if (bHasAcceleration) {
    // 角色在移动，重置空闲计时
    IdleElapsedTime = 0.f;
    bShouldEnterRelaxState = false;
  } else {
    // 角色静止，累加空闲时间
    IdleElapsedTime += DeltaSeconds;
    if (IdleElapsedTime >= EnterRelaxStateTime) {
      bShouldEnterRelaxState = true;
    }
  }
}