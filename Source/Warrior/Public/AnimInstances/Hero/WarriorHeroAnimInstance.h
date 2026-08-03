// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimInstances/WarriorCharacterAnimInstance.h"
#include "WarriorHeroAnimInstance.generated.h"

class AWarriorHeroCharacter;
/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorHeroAnimInstance : public UWarriorCharacterAnimInstance
{
	GENERATED_BODY()
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
protected:
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="AnimData|References")
	AWarriorHeroCharacter* OwningHeroCharacter;
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="AnimData|LocomotionData")
	bool bShouldEnterRelaxState;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="AnimData|LocomotionData")
	float EnterRelaxStateTime = 5.f;
	float IdleElapsedTime;
};
