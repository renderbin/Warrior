// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/WarriorEnemyCharacter.h"
#include "Components/Combat/EnemyCombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DataAssets/StartUpData/DataAsset_EnemyStartupData.h"
#include "Components/UI/EnemyUIComponent.h"
#include "Components/WidgetComponent.h"
#include "Widget/WarriorWidgetBase.h"
#include "Engine/AssetManager.h"
AWarriorEnemyCharacter::AWarriorEnemyCharacter() {
  AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
  bUseControllerRotationPitch = false;
  bUseControllerRotationRoll = false;
  bUseControllerRotationYaw = false;

  UCharacterMovementComponent *MovementComp = GetCharacterMovement();
  if (MovementComp) {
    MovementComp->bUseControllerDesiredRotation = false;
    MovementComp->bOrientRotationToMovement = true;
    MovementComp->RotationRate = FRotator(0.f, 180.f, 0.f);
    MovementComp->MaxWalkSpeed = 300.f;
    MovementComp->BrakingDecelerationWalking = 1000.f;

    EnemyHealthWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(
        TEXT("EnemyHealthWidgetComponent"));
    EnemyHealthWidgetComponent->SetupAttachment(GetMesh());
  }
  EnemyCombatComponent = CreateDefaultSubobject<UEnemyCombatComponent>(
      TEXT("EnemyCombatComponent"));
  EnemyUIComponent = CreateDefaultSubobject<UEnemyUIComponent>(
      TEXT("EnemyUIComponent"));
}

void AWarriorEnemyCharacter::BeginPlay() {
  Super::BeginPlay();

  // 从 WidgetComponent 获取控件对象并转换为 WarriorWidgetBase
  UWarriorWidgetBase *HealthWidget = Cast<UWarriorWidgetBase>(
      EnemyHealthWidgetComponent->GetUserWidgetObject());

  if (HealthWidget) {
    HealthWidget->InitEnemyCreatedWidget(this);
  }
}
void AWarriorEnemyCharacter::PossessedBy(AController *NewController) {
  Super::PossessedBy(NewController);
  InitEnemyStartUpData();
}
void AWarriorEnemyCharacter::InitEnemyStartUpData() {
  if (CharacterStartUpData.IsNull())
    return;
  UAssetManager::GetStreamableManager().RequestAsyncLoad(
      CharacterStartUpData.ToSoftObjectPath(),
      FStreamableDelegate::CreateLambda([this]() {
        UDataAsset_StartUpDataBase *LoadedData = CharacterStartUpData.Get();
        if (LoadedData)
          LoadedData->GiveToAbilitySystemComponent(
              GetWarriorAbilitySystemComponent());
      }));
}
UPawnCombatComponent *AWarriorEnemyCharacter::GetPawnCombatComponent() const {
  return EnemyCombatComponent;
}
UPawnUIComponent *AWarriorEnemyCharacter::GetPawnUIComponent() const {
  return EnemyUIComponent;
}

UEnemyUIComponent *AWarriorEnemyCharacter::GetEnemyUIComponent() const {
  return EnemyUIComponent; // 类中的 UI 组件成员变量
}