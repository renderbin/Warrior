// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstances/Hero/WarriorHeroAnimInstance.h"
#include "Characters/WarriorHeroCharacter.h"


void UWarriorHeroAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
    if(OwningCharacter)
    {
        OwningHeroCharacter = Cast<AWarriorHeroCharacter>(OwningCharacter);
    }
}
void UWarriorHeroAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
    if(bHasAcceleration)
    {
        IdleElapsedTime = 0.f;
        bShouldEnterRelaxState = false;
    }
    else{
        IdleElapsedTime += DeltaSeconds;
        if(IdleElapsedTime >= EnterRelaxStateTime)
        {
            bShouldEnterRelaxState = true;
        }
    }
}
