// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

UENUM(BlueprintType)
enum class EWarriorValidType : uint8
{
	Valid,
	Invalid
};

UENUM(BlueprintType)
enum class EToggleDamageType : uint8
{
	CurrentEquippedWeapon,
	LeftHand,
	RightHand,
	BothHands,
};

UENUM(BlueprintType)
enum class EWarriorSuccessType : uint8
{
	Failed,
	Successful
};