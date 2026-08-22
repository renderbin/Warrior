// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Characters/WarriorBaseCharacter.h"
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "WarriorHeroCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

class UDataAsset_InputConfig;
class UHeroCombatComponent;
class UHeroUIComponent;
struct FInputActionValue;

/**
 *
 */
UCLASS()
class WARRIOR_API AWarriorHeroCharacter : public AWarriorBaseCharacter {
  GENERATED_BODY()

public:
  AWarriorHeroCharacter();
  virtual UPawnCombatComponent *GetPawnCombatComponent() const override;
  virtual UPawnUIComponent *GetPawnUIComponent() const override;
  virtual UHeroUIComponent *GetHeroUIComponent() const override;

protected:
  virtual void SetupPlayerInputComponent(
      class UInputComponent *PlayerInputComponent) override;
  virtual void PossessedBy(AController *NewController) override;
    
private:
  void Input_Move(const FInputActionValue &InputActionValue);
  void Input_Look(const FInputActionValue &InputActionValue);

  void Input_AbilityInputPressed(FGameplayTag InInputTag);
  void Input_AbilityInputReleased(FGameplayTag InInputTag);

  UPROPERTY(VisibleAnywhere, Category = "Component",
            meta = (AllowPrivateAccess = "true"))
  USpringArmComponent *CameraBoom;

  UPROPERTY(VisibleAnywhere, Category = "Component",
            meta = (AllowPrivateAccess = "true"))
  UCameraComponent *FollowCamera;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData",
            meta = (AllowPrivateAccess = "true"))
  TObjectPtr<UDataAsset_InputConfig> InputConfigDataAsset;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat",
            meta = (AllowPrivateAccess = "true"))
  UHeroCombatComponent *HeroCombatComponent;
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI",
            meta = (AllowPrivateAccess = "true"))
  UHeroUIComponent *HeroUIComponent;

public:
  FORCEINLINE UHeroCombatComponent *GetHeroCombatComponent() const {
    return HeroCombatComponent;
  }
};
