// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "WarriorBaseCharacter.generated.h"

class UWarriorAbilitySystemComponent;
class UWarriorAttributeSet;
UCLASS()
class WARRIOR_API AWarriorBaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AWarriorBaseCharacter();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;
protected:	
	virtual void PossessedBy(AController* NewController) override;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="AbilitySystem")
	UWarriorAbilitySystemComponent* WarriorAbilitySystemComponent;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="AbilitySystem")
	UWarriorAttributeSet* WarriorAttributeSet;

public:
	FORCEINLINE UWarriorAbilitySystemComponent* GetWarriorAbilitySystemComponent() const
	{
		return WarriorAbilitySystemComponent;
	}
	FORCEINLINE UWarriorAttributeSet* GetWarriorAttributeSet() const
	{
		return WarriorAttributeSet;
	}

};
