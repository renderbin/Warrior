// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/WarriorHeroCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/Combat/HeroCombatComponent.h"
#include "Components/Input/WarriorInputComponent.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "DataAssets/StartUpData/DataAsset_HeroStartUpData.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/UI/HeroUIComponent.h"
#include "WarriorGameplayTags.h"

AWarriorHeroCharacter::AWarriorHeroCharacter() {
  // 胶囊体
  GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);
  bUseControllerRotationYaw = false;
  bUseControllerRotationPitch = false;
  bUseControllerRotationRoll = false;

  // SpringArm
  CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
  CameraBoom->SetupAttachment(GetRootComponent());
  CameraBoom->TargetArmLength = 200.f;                 // 摄像机距离
  CameraBoom->SocketOffset = FVector(0.f, 55.f, 65.f); // 过肩视角偏移
  CameraBoom->bUsePawnControlRotation = true;          // 悬臂跟随控制器旋转

  // Camera
  FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
  FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
  FollowCamera->bUsePawnControlRotation = false; // 摄像机不单独旋转

  // CharacterMovement
  GetCharacterMovement()->bOrientRotationToMovement = true;
  GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
  GetCharacterMovement()->MaxWalkSpeed = 400.f;
  GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
  HeroCombatComponent =
      CreateDefaultSubobject<UHeroCombatComponent>("HeroCombatComponent");
  HeroUIComponent = CreateDefaultSubobject<UHeroUIComponent>("HeroUIComponent");
}
void AWarriorHeroCharacter::SetupPlayerInputComponent(
    UInputComponent *PlayerInputComponent) {
  checkf(InputConfigDataAsset,
         TEXT("Forgot to assign a valid data asset as input config"));

  const ULocalPlayer *LocalPlayer =
      GetController<APlayerController>()->GetLocalPlayer();
  UEnhancedInputLocalPlayerSubsystem *Subsystem =
      ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
          LocalPlayer);
  check(Subsystem);
  Subsystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext, 0);

  UWarriorInputComponent *WarriorInputComponent =
      CastChecked<UWarriorInputComponent>(PlayerInputComponent);
  WarriorInputComponent->BindNativeInputAction(
      InputConfigDataAsset, WarriorGameplayTags::Input_Move,
      ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
  WarriorInputComponent->BindNativeInputAction(
      InputConfigDataAsset, WarriorGameplayTags::Input_Look,
      ETriggerEvent::Triggered, this, &ThisClass::Input_Look);

  WarriorInputComponent->BindAbilityInputAction(
      InputConfigDataAsset, this, &ThisClass::Input_AbilityInputPressed,
      &ThisClass::Input_AbilityInputReleased);
}

UPawnUIComponent *AWarriorHeroCharacter::GetPawnUIComponent() const {
  return HeroUIComponent;
}

UHeroUIComponent *AWarriorHeroCharacter::GetHeroUIComponent() const {
  return HeroUIComponent;
}

UPawnCombatComponent *AWarriorHeroCharacter::GetPawnCombatComponent() const {
  return HeroCombatComponent;
}
void AWarriorHeroCharacter::Input_Move(
    const FInputActionValue &InputActionValue) {
  const FVector2D MovementVector = InputActionValue.Get<FVector2D>();

  // 取控制器偏航角，忽略俯仰和翻滚
  const FRotator MovementRotation(
      0.f, GetController()->GetControlRotation().Yaw, 0.f);

  if (MovementVector.Y != 0.f) {
    const FVector ForwardDirection =
        MovementRotation.RotateVector(FVector::ForwardVector);
    AddMovementInput(ForwardDirection, MovementVector.Y);
  }

  if (MovementVector.X != 0.f) {
    const FVector RightDirection =
        MovementRotation.RotateVector(FVector::RightVector);
    AddMovementInput(RightDirection, MovementVector.X);
  }
}
void AWarriorHeroCharacter::Input_Look(
    const FInputActionValue &InputActionValue) {
  const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();

  if (LookAxisVector.X != 0.f) {
    AddControllerYawInput(LookAxisVector.X);
  }

  if (LookAxisVector.Y != 0.f) {
    AddControllerPitchInput(LookAxisVector.Y);
  }
}

void AWarriorHeroCharacter::PossessedBy(AController *NewController) {
  Super::PossessedBy(NewController);
  ensureMsgf(!CharacterStartUpData.IsNull(),
             TEXT("Forgot to assign start up data to %s"), *GetName());
  if (!CharacterStartUpData.IsNull()) {
    if (UDataAsset_StartUpDataBase *LoadedData =
            CharacterStartUpData.LoadSynchronous()) {
      LoadedData->GiveToAbilitySystemComponent(WarriorAbilitySystemComponent);
    }
  }
}

void AWarriorHeroCharacter::Input_AbilityInputPressed(FGameplayTag InInputTag) {
  if (!ensure(WarriorAbilitySystemComponent))
    return;
  WarriorAbilitySystemComponent->OnAbilityInputPressed(InInputTag);
}

void AWarriorHeroCharacter::Input_AbilityInputReleased(
    FGameplayTag InInputTag) {
  if (!ensure(WarriorAbilitySystemComponent))
    return;
  WarriorAbilitySystemComponent->OnAbilityInputReleased(InInputTag);
}