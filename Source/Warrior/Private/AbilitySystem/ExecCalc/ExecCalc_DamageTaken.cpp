// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalc/ExecCalc_DamageTaken.h"
#include "AbilitySystem/WarriorAttributeSet.h"
#include "WarriorDebugHelper.h"
#include "WarriorGameplayTags.h"
struct FWDamageCapture {
  DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower);
  DECLARE_ATTRIBUTE_CAPTUREDEF(DefensePower);

  DECLARE_ATTRIBUTE_CAPTUREDEF(DamageTaken);
  FWDamageCapture() {
    DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet, AttackPower, Source, false);
    DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet, DefensePower, Target, false);

    DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet, DamageTaken, Target, false);
  }
};

static const FWDamageCapture &GetDamageCapture() {
  static FWDamageCapture DamageCapture;
  return DamageCapture;
}
UExecCalc_DamageTaken::UExecCalc_DamageTaken() {
  RelevantAttributesToCapture.Add(GetDamageCapture().AttackPowerDef);
  RelevantAttributesToCapture.Add(GetDamageCapture().DefensePowerDef);

  RelevantAttributesToCapture.Add(GetDamageCapture().DamageTakenDef);
}
void UExecCalc_DamageTaken::Execute_Implementation(
    const FGameplayEffectCustomExecutionParameters &ExecutionParams,
    FGameplayEffectCustomExecutionOutput &OutExecutionOutput) const {
  const FGameplayEffectSpec &EffectSpec = ExecutionParams.GetOwningSpec();

  // EffectSpec.GetContext().GetSourceObject();
  // EffectSpec.GetContext().GetAbility();
  // EffectSpec.GetContext().GetInstigator();
  // EffectSpec.GetContext().GetEffectCauser();

  FAggregatorEvaluateParameters EvaluateParams;
  EvaluateParams.SourceTags = EffectSpec.CapturedSourceTags.GetAggregatedTags();
  EvaluateParams.TargetTags = EffectSpec.CapturedTargetTags.GetAggregatedTags();

  float SourceAttackPower = 0.f;
  ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
      GetDamageCapture().AttackPowerDef, EvaluateParams, SourceAttackPower);

  float BaseDamage = 0.f;
  int32 UsedLightAttackComboCount = 0;
  int32 UsedHeavyAttackComboCount = 0;

  for (const auto &TagMagnitude : EffectSpec.SetByCallerTagMagnitudes) {
    if (TagMagnitude.Key.MatchesTagExact(
            WarriorGameplayTags::Shared_SetByCaller_BaseDamage)) {
      BaseDamage = TagMagnitude.Value;
    }

    if (TagMagnitude.Key.MatchesTagExact(
            WarriorGameplayTags::AttackType_Light)) {
      UsedLightAttackComboCount = TagMagnitude.Value;
    }

    if (TagMagnitude.Key.MatchesTagExact(
            WarriorGameplayTags::AttackType_Heavy)) {
      UsedHeavyAttackComboCount = TagMagnitude.Value;
    }
  }

  float TargetDefensePower = 0.f;
  ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
      GetDamageCapture().DefensePowerDef, EvaluateParams, TargetDefensePower);

  if (UsedLightAttackComboCount != 0) {
    const float DamageIncreasePercentLight =
        (UsedLightAttackComboCount - 1) * 0.05f + 1.f;
    BaseDamage *= DamageIncreasePercentLight;
  }

  if (UsedHeavyAttackComboCount != 0) {
    const float DamageIncreasePercentHeavy =
        UsedHeavyAttackComboCount * 0.15f + 1.f;
    BaseDamage *= DamageIncreasePercentHeavy;
  }

  Debug::Print(TEXT("BaseDamage"), BaseDamage);
  Debug::Print(TEXT("SourceAttackPower"), SourceAttackPower);
  Debug::Print(TEXT("TargetDefensePower"), TargetDefensePower);

  const float FinalDamageDone =
      BaseDamage * SourceAttackPower / TargetDefensePower;
  Debug::Print(TEXT("FinalDamageDone"), FinalDamageDone);
  if (FinalDamageDone > 0.f) {
    OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
        GetDamageCapture().DamageTakenProperty, EGameplayModOp::Override,
        FinalDamageDone));
  }
}