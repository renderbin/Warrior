// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/WarriorBaseCharacter.h"
#include "Interfaces/PawnCombatInterface.h"

#include "WarriorEnemyCharacter.generated.h"
class UEnemyUIComponent;
class UEnemyCombatComponent;
class UWidgetComponent;
/**
 *
 */
UCLASS()
class WARRIOR_API AWarriorEnemyCharacter : public AWarriorBaseCharacter {
  GENERATED_BODY()

public:
  AWarriorEnemyCharacter();
  virtual void PossessedBy(AController *NewController) override;
  virtual void BeginPlay() override;
  void InitEnemyStartUpData();
  virtual UPawnUIComponent *GetPawnUIComponent() const override;
  virtual UEnemyUIComponent *GetEnemyUIComponent() const override;
  virtual UPawnCombatComponent *GetPawnCombatComponent() const override;

  FORCEINLINE UEnemyCombatComponent *GetEnemyCombatComponent() const {
    return EnemyCombatComponent;
  }

protected:
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
  UEnemyCombatComponent *EnemyCombatComponent;
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category ="UI")
  UEnemyUIComponent *EnemyUIComponent;
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
  UWidgetComponent *EnemyHealthWidgetComponent;
};
