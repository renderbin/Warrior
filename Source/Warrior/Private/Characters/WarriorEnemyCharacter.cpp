// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/WarriorEnemyCharacter.h"
#include "Components/Combat/EnemyCombatComponent.h"
#include "Components/UI/EnemyUIComponent.h"
#include "Components/UI/PawnUIComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include "Components/WidgetComponent.h"
#include "Widgets/WarriorWidgetBase.h"
AWarriorEnemyCharacter::AWarriorEnemyCharacter()
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	if (MovementComp)
	{
		MovementComp->bUseControllerDesiredRotation = false;
		MovementComp->bOrientRotationToMovement = true;
		MovementComp->RotationRate = FRotator(0.f, 180.f, 0.f);
		MovementComp->MaxWalkSpeed = 300.f;
		MovementComp->BrakingDecelerationWalking = 1000.f;
	}

	EnemyCombatComponent = CreateDefaultSubobject<UEnemyCombatComponent>(TEXT("EnemyCombatComponent"));
	EnemyUIComponent = CreateDefaultSubobject<UEnemyUIComponent>(TEXT("EnemyUIComponent"));

	EnemyHealthWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("EnemyHealthWidgetComponent"));
	EnemyHealthWidgetComponent->SetupAttachment(GetMesh());
}


void AWarriorEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitEnemyStartUpData();
}

UPawnCombatComponent* AWarriorEnemyCharacter::GetPawnCombatComponent() const
{
	return EnemyCombatComponent;
}

UPawnUIComponent* AWarriorEnemyCharacter::GetPawnUIComponent() const
{
	return EnemyUIComponent;
}

void AWarriorEnemyCharacter::InitEnemyStartUpData()
{
	if (CharacterStartUpData.IsNull())
		return;
	UAssetManager::GetStreamableManager().RequestAsyncLoad(
	    CharacterStartUpData.ToSoftObjectPath(), FStreamableDelegate::CreateLambda([this]() {
		    UDataAsset_StartUpDataBase* LoadedData = CharacterStartUpData.Get();
		    if (LoadedData)
			    LoadedData->GiveToAbilitySystemComponent(GetWarriorAbilitySystemComponent());
	    }));
}

UEnemyUIComponent* AWarriorEnemyCharacter::GetEnemyUIComponent() const
{
	return EnemyUIComponent; // 类中的 UI 组件成员变量
}

void AWarriorEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	UWarriorWidgetBase* HealthWidget = Cast<UWarriorWidgetBase>(EnemyHealthWidgetComponent->GetUserWidgetObject());
	if (HealthWidget)
	{
		HealthWidget->InitEnemyCreatedWidget(this);
	}
}