// Fill out your copyright notice in the Description page of Project Settings.

#include "Interfaces/PawnUIInterface.h"
#include "Components/UI/HeroUIComponent.h"
#include "Components/UI/EnemyUIComponent.h"

UHeroUIComponent* IPawnUIInterface::GetHeroUIComponent() const
{
	return nullptr;
}

UEnemyUIComponent* IPawnUIInterface::GetEnemyUIComponent() const
{
	return nullptr;
}
