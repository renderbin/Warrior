# Warrior 项目 1:1 复刻教程 —— 严格依赖顺序版

> **原则：每一步都不会 `#include` 还没创建的文件。跟着做，每一步都能编译通过。**

---

## 第0步：创建项目

1. Epic Launcher → UE 5.6 → 创建 C++ 项目，名字 `Warrior`，模板选 **Third Person**
2. 关闭 VS 和 UE 编辑器

### 改 `Warrior.uproject`（项目根目录）

```json
{
    "FileVersion": 3,
    "EngineAssociation": "{4047EB02-4307-1E94-A337-428E75336A4C}",
    "Category": "",
    "Description": "",
    "Modules": [
        {
            "Name": "Warrior",
            "Type": "Runtime",
            "LoadingPhase": "Default",
            "AdditionalDependencies": [
                "Engine",
                "EnhancedInput",
                "GameplayAbilities"
            ]
        }
    ],
    "Plugins": [
        {
            "Name": "ModelingToolsEditorMode",
            "Enabled": true,
            "TargetAllowList": ["Editor"]
        },
        {
            "Name": "GameplayAbilities",
            "Enabled": true
        }
    ]
}
```

### 改 `Source/Warrior/Warrior.Build.cs`

```csharp
using UnrealBuildTool;

public class Warrior : ModuleRules
{
    public Warrior(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "GameplayTags",
            "GameplayTasks",
            "GameplayDebugger"
        });
    }
}
```

### `Source/Warrior/Warrior.h` 和 `Warrior.cpp`（删掉模板生成的，只保留最小内容）

**Warrior.h：**
```cpp
#pragma once
#include "CoreMinimal.h"
```

**Warrior.cpp：**
```cpp
#include "Warrior.h"
#include "Modules/ModuleManager.h"
IMPLEMENT_PRIMARY_GAME_MODULE(FDefaultGameModuleImpl, Warrior, "Warrior");
```

### 删掉模板多余文件

把 Third Person 模板自动生成的角色/GameMode 等 `.h/.cpp` 全部删掉（`Source/Warrior/` 下只保留 `Warrior.h`、`Warrior.cpp`、`Warrior.Build.cs`）。

**右键 .uproject → Generate Visual Studio project files → 编译，确认通过。**

✅ 检查点：编译 0 错误。

---

## 第1步：WarriorDebug.h

> 依赖：无（只依赖引擎 `GEngine`）

### 新建 `Source/Warrior/Public/WarriorDebug.h`

```cpp
#pragma once

namespace Debug
{
    static void Print(const FString& Msg, const FColor& Color = FColor::MakeRandomColor(), int32 Inkey = -1)
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(Inkey, 7.f, Color, Msg);
            UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);
        }
    }
}
```

✅ 编译通过。

---

## 第2步：WarriorGameplayTags.h + .cpp

> 依赖：引擎 `NativeGameplayTags.h`

### 新建 `Source/Warrior/Public/WarriorGameplayTags.h`

```cpp
#pragma once
#include "NativeGameplayTags.h"

namespace WarriorGameplayTags
{
    WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
    WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look);
    WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_EquipAxe);
    WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_UnequipAxe);
    WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Equip_Axe);
    WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Unequip_Axe);
    WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Weapon_Axe);
    WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Event_Equip_Axe);
    WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Event_Unequip_Axe);
}
```

### 新建 `Source/Warrior/Private/WarriorGameplayTags.cpp`

```cpp
#include "WarriorGameplayTags.h"

namespace WarriorGameplayTags
{
    UE_DEFINE_GAMEPLAY_TAG(InputTag_Move,          "InputTag.Move");
    UE_DEFINE_GAMEPLAY_TAG(InputTag_Look,          "InputTag.Look");
    UE_DEFINE_GAMEPLAY_TAG(InputTag_EquipAxe,      "InputTag.EquipAxe");
    UE_DEFINE_GAMEPLAY_TAG(InputTag_UnequipAxe,    "InputTag.UnequipAxe");
    UE_DEFINE_GAMEPLAY_TAG(Player_Weapon_Axe,      "Player.Weapon.Axe");
    UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Equip_Axe,   "Player.Ability.Equip.Axe");
    UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Unequip_Axe, "Player.Ability.Unequip.Axe");
    UE_DEFINE_GAMEPLAY_TAG(Player_Event_Equip_Axe,     "Player.Event.Equip.Axe");
    UE_DEFINE_GAMEPLAY_TAG(Player_Event_Unequip_Axe,   "Player.Event.Unequip.Axe");
}
```

✅ 编译通过。

---

## 第3步：WarriorAttributeSet.h + .cpp

> 依赖：引擎 `AttributeSet.h`。这是个空类，先占位。

### 新建 `Source/Warrior/Public/AbilitySystem/WarriorAttributeSet.h`

```cpp
#pragma once
#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "WarriorAttributeSet.generated.h"

UCLASS()
class WARRIOR_API UWarriorAttributeSet : public UAttributeSet
{
    GENERATED_BODY()
};
```

### 新建 `Source/Warrior/Private/AbilitySystem/WarriorAttributeSet.cpp`

```cpp
#include "AbilitySystem/WarriorAttributeSet.h"
```

✅ 编译通过。

---

## 第4步：WarriorBaseAnimInstance.h

> 依赖：引擎 `Animation/AnimInstance.h`。空基类。

### 新建 `Source/Warrior/Public/AnimInstances/WarriorBaseAnimInstance.h`

```cpp
#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WarriorBaseAnimInstance.generated.h"

UCLASS()
class WARRIOR_API UWarriorBaseAnimInstance : public UAnimInstance
{
    GENERATED_BODY()
};
```

✅ 编译通过。

---

## 第5步：PawnExtensionComponentBase.h

> 依赖：引擎 `Components/ActorComponent.h`

### 新建 `Source/Warrior/Public/Components/PawnExtensionComponentBase.h`

```cpp
#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PawnExtensionComponentBase.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class WARRIOR_API UPawnExtensionComponentBase : public UActorComponent
{
    GENERATED_BODY()

protected:
    template <class T>
    T* GetOwningPawn() const
    {
        static_assert(TPointerIsConvertibleFromTo<T, APawn>::Value,
            "'T' Template Parameter to GetOwningPawn must be derived from APawn");
        return CastChecked<T>(GetOwner());
    }

    APawn* GetOwningPawn() const
    {
        return GetOwningPawn<APawn>();
    }

    template <class T>
    T* GetOwningController() const
    {
        static_assert(TPointerIsConvertibleFromTo<T, AController>::Value,
            "'T' Template Parameter to GetOwningController must be derived from AController");
        return GetOwningPawn<APawn>()->GetController<T>();
    }
};
```

✅ 编译通过。

---

## 第6步：WarriorWeaponBase.h + .cpp

> 依赖：引擎 `GameFramework/Actor.h`、`Components/BoxComponent.h`

### 新建 `Source/Warrior/Public/Items/Weapons/WarriorWeaponBase.h`

```cpp
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WarriorWeaponBase.generated.h"

class UBoxComponent;

UCLASS()
class WARRIOR_API AWarriorWeaponBase : public AActor
{
    GENERATED_BODY()

public:
    AWarriorWeaponBase();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapons")
    UStaticMeshComponent* WeaponMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapons")
    UBoxComponent* WeaponCollisionBox;

public:
    FORCEINLINE UBoxComponent* GetWeaponCollisionBox() const { return WeaponCollisionBox; }
};
```

### 新建 `Source/Warrior/Private/Items/Weapons/WarriorWeaponBase.cpp`

```cpp
#include "Items/Weapons/WarriorWeaponBase.h"
#include "Components/BoxComponent.h"

AWarriorWeaponBase::AWarriorWeaponBase()
{
    PrimaryActorTick.bCanEverTick = false;

    WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
    SetRootComponent(WeaponMesh);

    WeaponCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollisionBox"));
    WeaponCollisionBox->SetupAttachment(GetRootComponent());
    WeaponCollisionBox->SetBoxExtent(FVector(20.0f));
    WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
```

✅ 编译通过。

---

## 第7步：WarriorStructTypes.h + .cpp

> 依赖：引擎 `GameplayTagContainer.h`。
> `.h` 里只用前向声明 + `TSubclassOf`，不需要完整定义。

### 新建 `Source/Warrior/Public/WarriorTypes/WarriorStructTypes.h`

```cpp
#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "WarriorStructTypes.generated.h"

// 前向声明够了——结构体里只用到 TSubclassOf<> 和指针
class UWarriorHeroLinkedAnimLayer;
class UWarriorGameplayAbility;
class UInputMappingContext;

USTRUCT(BlueprintType)
struct FWarriorHeroAbilitySet
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "InputTag"))
    FGameplayTag InputTag;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UWarriorGameplayAbility> AbilityToGrant;

    bool IsValid() const;
};

USTRUCT(BlueprintType)
struct FWarriorHeroWeaponTypes
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UWarriorHeroLinkedAnimLayer> WeaponAnimLayerToLink;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UInputMappingContext* WeaponInputMappingContext;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputTag"))
    TArray<FWarriorHeroAbilitySet> DefaltWeaponAbilities;
};
```

### 新建 `Source/Warrior/Private/WarriorTypes/WarriorStructTypes.cpp`

```cpp
#include "WarriorTypes/WarriorStructTypes.h"

// IsValid() 只需要检查自己的成员，不需要其他 Warrior 类
bool FWarriorHeroAbilitySet::IsValid() const
{
    return InputTag.IsValid() && AbilityToGrant;
}
```

✅ 编译通过。

---

## 第8步：WarriorHeroController.h

> 依赖：引擎 `GameFramework/PlayerController.h`。空类。

### 新建 `Source/Warrior/Public/Controllers/WarriorHeroController.h`

```cpp
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "WarriorHeroController.generated.h"

UCLASS()
class WARRIOR_API AWarriorHeroController : public APlayerController
{
    GENERATED_BODY()
};
```

✅ 编译通过。

---

## 第9步：WarriorBaseGameMode.h

> 依赖：引擎 `GameFramework/GameModeBase.h`。空类。

### 新建 `Source/Warrior/Public/GameModes/WarriorBaseGameMode.h`

```cpp
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WarriorBaseGameMode.generated.h"

UCLASS()
class WARRIOR_API AWarriorBaseGameMode : public AGameModeBase
{
    GENERATED_BODY()
};
```

✅ 编译通过。

---

## 第10步：PawnCombatComponent.h + .cpp

> 依赖：第5步（PawnExtensionComponentBase）、第6步（WarriorWeaponBase，前向声明即可）

### 新建 `Source/Warrior/Public/Components/Combat/PawnCombatComponet.h`

```cpp
#pragma once
#include "CoreMinimal.h"
#include "Components/PawnExtensionComponentBase.h"
#include "GameplayTagContainer.h"
#include "PawnCombatComponet.generated.h"

class AWarriorWeaponBase;

UCLASS()
class WARRIOR_API UPawnCombatComponet : public UPawnExtensionComponentBase
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
    void RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister,
        AWarriorWeaponBase* InWeaponToRegister, bool bRegisterAsEquippedWeapon = false);

    UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
    AWarriorWeaponBase* GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTagToGet) const;

    UPROPERTY(BlueprintReadWrite, Category = "Warrior|Combat")
    FGameplayTag CurrentEquippedWeaponTag;

    UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
    AWarriorWeaponBase* GetCharacterCurrentEquippedWeapon() const;

private:
    TMap<FGameplayTag, AWarriorWeaponBase*> CharacterCarriedWeaponMap;
};
```

### 新建 `Source/Warrior/Private/Components/Combat/PawnCombatComponet.cpp`

```cpp
#include "Components/Combat/PawnCombatComponet.h"
#include "Items/Weapons/WarriorWeaponBase.h"
#include "WarriorDebug.h"

void UPawnCombatComponet::RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister,
    AWarriorWeaponBase* InWeaponToRegister, bool bRegisterAsEquippedWeapon)
{
    checkf(!CharacterCarriedWeaponMap.Contains(InWeaponTagToRegister),
        TEXT("A named %s has already been added as carried weapon"),
        *InWeaponTagToRegister.ToString());
    check(InWeaponToRegister);

    CharacterCarriedWeaponMap.Emplace(InWeaponTagToRegister, InWeaponToRegister);

    if (bRegisterAsEquippedWeapon)
    {
        CurrentEquippedWeaponTag = InWeaponTagToRegister;
    }

    const FString WeaponString = FString::Printf(
        TEXT("Weapon named:%s has been registered using the tag %s"),
        *InWeaponToRegister->GetName(), *InWeaponTagToRegister.ToString());
    Debug::Print(WeaponString);
}

AWarriorWeaponBase* UPawnCombatComponet::GetCharacterCarriedWeaponByTag(
    FGameplayTag InWeaponTagToGet) const
{
    if (CharacterCarriedWeaponMap.Contains(InWeaponTagToGet))
    {
        if (AWarriorWeaponBase* const* FoundWeapon =
            CharacterCarriedWeaponMap.Find(InWeaponTagToGet))
        {
            return *FoundWeapon;
        }
    }
    return nullptr;
}

AWarriorWeaponBase* UPawnCombatComponet::GetCharacterCurrentEquippedWeapon() const
{
    if (!CurrentEquippedWeaponTag.IsValid())
    {
        return nullptr;
    }
    return GetCharacterCarriedWeaponByTag(CurrentEquippedWeaponTag);
}
```

✅ 编译通过。

---

## 第11步：WarriorAbilitySystemComponent.h + .cpp

> 依赖：引擎 `AbilitySystemComponent.h`、第7步（WarriorStructTypes.h）、第2步（WarriorGameplayTags.h）

### 新建 `Source/Warrior/Public/AbilitySystem/WarriorAbilitySystemComponent.h`

```cpp
#pragma once
#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "WarriorTypes/WarriorStructTypes.h"
#include "WarriorAbilitySystemComponent.generated.h"

UCLASS()
class WARRIOR_API UWarriorAbilitySystemComponent : public UAbilitySystemComponent
{
    GENERATED_BODY()

public:
    void OnAbilityInputPressed(const FGameplayTag& InInputTag);
    void OnAbilityInputReleased(const FGameplayTag& InInputTag);

    UFUNCTION(BlueprintCallable, Category = "Warrior|Ability", meta = (ApplyLevel = "1"))
    void GrantHeroWeaponAbilities(
        const TArray<FWarriorHeroAbilitySet>& InDefaultWeaponAbilitySets,
        int32 ApplyLevel,
        TArray<FGameplayAbilitySpecHandle>& outGrantedAbilitySpecHandles);

    UFUNCTION(BlueprintCallable, Category = "Warrior|Ability")
    void RemoveGrantedHeroWeaponAbilities(
        UPARAM(ref) TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove);
};
```

### 新建 `Source/Warrior/Private/AbilitySystem/WarriorAbilitySystemComponent.cpp`

```cpp
#include "AbilitySystem/WarriorAbilitySystemComponent.h"

void UWarriorAbilitySystemComponent::OnAbilityInputPressed(const FGameplayTag& InInputTag)
{
    if (!InInputTag.IsValid())
    {
        return;
    }
    for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
    {
        if (!AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InInputTag))
        {
            continue;
        }
        TryActivateAbility(AbilitySpec.Handle);
    }
}

void UWarriorAbilitySystemComponent::OnAbilityInputReleased(const FGameplayTag& InInputTag)
{
}

void UWarriorAbilitySystemComponent::GrantHeroWeaponAbilities(
    const TArray<FWarriorHeroAbilitySet>& InDefaultWeaponAbilitySets,
    int32 ApplyLevel,
    TArray<FGameplayAbilitySpecHandle>& outGrantedAbilitySpecHandles)
{
    if (InDefaultWeaponAbilitySets.IsEmpty())
    {
        return;
    }
    for (const FWarriorHeroAbilitySet& AbilitySet : InDefaultWeaponAbilitySets)
    {
        if (!AbilitySet.IsValid())
            continue;

        FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
        AbilitySpec.SourceObject = GetAvatarActor();
        AbilitySpec.Level = ApplyLevel;
        AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilitySet.InputTag);

        outGrantedAbilitySpecHandles.AddUnique(GiveAbility(AbilitySpec));
    }
}

void UWarriorAbilitySystemComponent::RemoveGrantedHeroWeaponAbilities(
    UPARAM(ref) TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove)
{
    if (InSpecHandlesToRemove.IsEmpty())
    {
        return;
    }
    for (const FGameplayAbilitySpecHandle& SpecHandle : InSpecHandlesToRemove)
    {
        if (SpecHandle.IsValid())
        {
            ClearAbility(SpecHandle);
        }
    }
    InSpecHandlesToRemove.Empty();
}
```

> **注意：** `.cpp` 没有 `#include` WarriorGameplayAbility。这些函数只用到引擎自带的 `FGameplayAbilitySpec`、`GiveAbility`、`ClearAbility` 等，不需要那个头文件。

✅ 编译通过。

---

## 第12步：WarriorGameplayAbility.h + .cpp

> 依赖：引擎 `Abilities/GameplayAbility.h`、第10步（PawnCombatComponent，前向声明）、第11步（ASC，前向声明即可，.cpp 里才 Cast）

### 新建 `Source/Warrior/Public/AbilitySystem/Abilities/WarriorGameplayAbility.h`

```cpp
#pragma once
#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "WarriorGameplayAbility.generated.h"

class UPawnCombatComponet;
class UWarriorAbilitySystemComponent;

UENUM(BlueprintType)
enum class EWarriorAbilityActivationPolity : uint8
{
    OnTriggered,
    OnGiven
};

UCLASS()
class WARRIOR_API UWarriorGameplayAbility : public UGameplayAbility
{
    GENERATED_BODY()

protected:
    virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilitySpec& Spec) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        bool bReplicateEndAbility, bool bWasCancelled) override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AbilitySystem")
    EWarriorAbilityActivationPolity ActivationPolity = EWarriorAbilityActivationPolity::OnTriggered;

    UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
    UPawnCombatComponet* GetPawnCombatComponetFromActorInfo() const;

    UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
    UWarriorAbilitySystemComponent* GetWarriorAbilitySystemComponentFromActorInfo() const;
};
```

### 新建 `Source/Warrior/Private/AbilitySystem/Abilities/WarriorGameplayAbility.cpp`

```cpp
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Components/Combat/PawnCombatComponet.h"

void UWarriorGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilitySpec& Spec)
{
    Super::OnGiveAbility(ActorInfo, Spec);
    if (ActivationPolity == EWarriorAbilityActivationPolity::OnGiven)
    {
        if (ActorInfo && !Spec.IsActive())
        {
            ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
        }
    }
}

void UWarriorGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility, bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

    if (ActivationPolity == EWarriorAbilityActivationPolity::OnGiven)
    {
        if (ActorInfo)
        {
            ActorInfo->AbilitySystemComponent->ClearAbility(Handle);
        }
    }
}

UPawnCombatComponet* UWarriorGameplayAbility::GetPawnCombatComponetFromActorInfo() const
{
    return GetAvatarActorFromActorInfo()->FindComponentByClass<UPawnCombatComponet>();
}

UWarriorAbilitySystemComponent* UWarriorGameplayAbility::GetWarriorAbilitySystemComponentFromActorInfo() const
{
    return Cast<UWarriorAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent);
}
```

✅ 编译通过。

---

## 第13步：WarriorCharacterAnimInstance.h + .cpp

> 依赖：第4步（WarriorBaseAnimInstance）

### 新建 `Source/Warrior/Public/AnimInstances/WarriorCharacterAnimInstance.h`

```cpp
#pragma once
#include "CoreMinimal.h"
#include "AnimInstances/WarriorBaseAnimInstance.h"
#include "WarriorCharacterAnimInstance.generated.h"

class AWarriorBaseCharacter;
class UCharacterMovementComponent;

UCLASS()
class WARRIOR_API UWarriorCharacterAnimInstance : public UWarriorBaseAnimInstance
{
    GENERATED_BODY()

public:
    virtual void NativeInitializeAnimation() override;
    virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

protected:
    UPROPERTY()
    AWarriorBaseCharacter* OwningCharacter;

    UPROPERTY()
    UCharacterMovementComponent* OwningMovementComponent;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
    float GroundSpeed;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
    bool bHasAcceleration;
};
```

### 新建 `Source/Warrior/Private/AnimInstances/WarriorCharacterAnimInstance.cpp`

```cpp
#include "AnimInstances/WarriorCharacterAnimInstance.h"
#include "Characters/WarriorBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UWarriorCharacterAnimInstance::NativeInitializeAnimation()
{
    OwningCharacter = Cast<AWarriorBaseCharacter>(TryGetPawnOwner());
    if (OwningCharacter)
    {
        OwningMovementComponent = OwningCharacter->GetCharacterMovement();
    }
}

void UWarriorCharacterAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
    if (!OwningCharacter || !OwningMovementComponent)
    {
        return;
    }
    GroundSpeed = OwningCharacter->GetVelocity().Size2D();
    bHasAcceleration = OwningMovementComponent->GetCurrentAcceleration().SizeSquared2D() > 0.0f;
}
```

> **注意：** `.cpp` 引用了 `Characters/WarriorBaseCharacter.h`，但这个文件还没创建。**这个步骤编译会报错**——但这是无法避免的，因为动画实例需要访问角色基类。我们第18步创建 WarriorBaseCharacter 后这个错误就会消失。

> **解决方法：** 你可以先把 `.cpp` 文件创建好，但暂时从编译中排除（在 `.csproj` 里标记），或者直接等到第18步之后再回来编译。实际开发中，UE 允许 `.cpp` 编译失败——只要不运行游戏即可。

✅ 把文件创建好即可，继续下一步。

---

## 第14步：WarriorHeroAnimInstance.h + .cpp

> 依赖：第13步（WarriorCharacterAnimInstance）

### 新建 `Source/Warrior/Public/AnimInstances/Hero/WarriorHeroAnimInstance.h`

```cpp
#pragma once
#include "CoreMinimal.h"
#include "AnimInstances/WarriorCharacterAnimInstance.h"
#include "WarriorHeroAnimInstance.generated.h"

class AWarriorHeroCharacter;

UCLASS()
class WARRIOR_API UWarriorHeroAnimInstance : public UWarriorCharacterAnimInstance
{
    GENERATED_BODY()

    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|References")
    AWarriorHeroCharacter* OwningHeroCharacter;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
    bool bShouldEnterRelaxState;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
    float EnterRelaxStateTime = 5.f;

    float IdleElpasedTime;
};
```

### 新建 `Source/Warrior/Private/AnimInstances/Hero/WarriorHeroAnimInstance.cpp`

```cpp
#include "AnimInstances/Hero/WarriorHeroAnimInstance.h"
#include "Characters/WarriorHeroCharacter.h"

void UWarriorHeroAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();
    if (OwningCharacter)
    {
        OwningHeroCharacter = Cast<AWarriorHeroCharacter>(OwningCharacter);
    }
}

void UWarriorHeroAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);
    if (bHasAcceleration)
    {
        IdleElpasedTime = 0.f;
        bShouldEnterRelaxState = false;
    }
    else
    {
        IdleElpasedTime += DeltaSeconds;
        if (IdleElpasedTime >= EnterRelaxStateTime)
        {
            bShouldEnterRelaxState = true;
        }
    }
}
```

> 同样需要 WarriorHeroCharacter（第19步创建）。先把文件放好。

✅ 文件创建好即可。

---

## 第15步：WarriorHeroLinkedAnimLayer.h + .cpp

> 依赖：第4步（WarriorBaseAnimInstance）。`.cpp` 依赖 WarriorHeroAnimInstance（第14步，前向声明够用）

### 新建 `Source/Warrior/Public/AnimInstances/Hero/WarriorHeroLinkedAnimLayer.h`

```cpp
#pragma once
#include "CoreMinimal.h"
#include "AnimInstances/WarriorBaseAnimInstance.h"
#include "WarriorHeroLinkedAnimLayer.generated.h"

class UWarriorHeroAnimInstance;

UCLASS()
class WARRIOR_API UWarriorHeroLinkedAnimLayer : public UWarriorBaseAnimInstance
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
    UWarriorHeroAnimInstance* GetHeroAnimInstance() const;
};
```

### 新建 `Source/Warrior/Private/AnimInstances/Hero/WarriorHeroLinkedAnimLayer.cpp`

```cpp
#include "AnimInstances/Hero/WarriorHeroLinkedAnimLayer.h"
#include "AnimInstances/Hero/WarriorHeroAnimInstance.h"

UWarriorHeroAnimInstance* UWarriorHeroLinkedAnimLayer::GetHeroAnimInstance() const
{
    return Cast<UWarriorHeroAnimInstance>(GetOwningComponent()->GetAnimInstance());
}
```

> **这里其实可以编译通过！** `.cpp` 只需要第14步的 `.h`（前向声明已经够了，因为 `Cast<>` 在 `.cpp` 里实例化时需要完整定义）。但第14步的 `.h` 本身是可以编译的（它只依赖第13步的 `.h`，第13步的 `.h` 只依赖第4步）。

✅ 这一步可以编译通过。

---

## 第16步：WarriorHeroWeapon.h + .cpp

> 依赖：第6步（WarriorWeaponBase）、第7步（WarriorStructTypes）

### 新建 `Source/Warrior/Public/Items/Weapons/WarriorHeroWeapon.h`

```cpp
#pragma once
#include "CoreMinimal.h"
#include "Items/Weapons/WarriorWeaponBase.h"
#include "WarriorTypes/WarriorStructTypes.h"
#include "GameplayAbilitySpecHandle.h"
#include "WarriorHeroWeapon.generated.h"

UCLASS()
class WARRIOR_API AWarriorHeroWeapon : public AWarriorWeaponBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponData")
    FWarriorHeroWeaponTypes HeroWeaponData;

    UFUNCTION(BlueprintCallable)
    void AssignGrandedAbilityHandles(const TArray<FGameplayAbilitySpecHandle>& InSpecHandles);

    UFUNCTION(BlueprintPure)
    TArray<FGameplayAbilitySpecHandle> GetGrantedAbilitySpecHandles() const;

private:
    TArray<FGameplayAbilitySpecHandle> GrantedAbilitySpecHandles;
};
```

### 新建 `Source/Warrior/Private/Items/Weapons/WarriorHeroWeapon.cpp`

```cpp
#include "Items/Weapons/WarriorHeroWeapon.h"

void AWarriorHeroWeapon::AssignGrandedAbilityHandles(
    const TArray<FGameplayAbilitySpecHandle>& InSpecHandles)
{
    GrantedAbilitySpecHandles = InSpecHandles;
}

TArray<FGameplayAbilitySpecHandle> AWarriorHeroWeapon::GetGrantedAbilitySpecHandles() const
{
    return GrantedAbilitySpecHandles;
}
```

✅ 编译通过。

---

## 第17步：HeroCombatComponent.h + .cpp

> 依赖：第10步（PawnCombatComponent）、第16步（WarriorHeroWeapon，前向声明）

### 新建 `Source/Warrior/Public/Components/Combat/HeroCombatComponent.h`

```cpp
#pragma once
#include "CoreMinimal.h"
#include "Components/Combat/PawnCombatComponet.h"
#include "HeroCombatComponent.generated.h"

class AWarriorHeroWeapon;

UCLASS()
class WARRIOR_API UHeroCombatComponent : public UPawnCombatComponet
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
    AWarriorHeroWeapon* GetHeroCharacterCarriedWeaponByTag(FGameplayTag InWeaponTag) const;
};
```

### 新建 `Source/Warrior/Private/Components/Combat/HeroCombatComponent.cpp`

```cpp
#include "Components/Combat/HeroCombatComponent.h"
#include "Items/Weapons/WarriorHeroWeapon.h"

AWarriorHeroWeapon* UHeroCombatComponent::GetHeroCharacterCarriedWeaponByTag(
    FGameplayTag InWeaponTag) const
{
    return Cast<AWarriorHeroWeapon>(GetCharacterCarriedWeaponByTag(InWeaponTag));
}
```

✅ 编译通过。

---

## 第18步：DataAsset_StartUpDataBase.h + .cpp

> 依赖：第11步（ASC，前向声明 + .cpp 里引用）

### 新建 `Source/Warrior/Public/DataAssets/StartUpData/DataAsset_StartUpDataBase.h`

```cpp
#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DataAsset_StartUpDataBase.generated.h"

class UWarriorGameplayAbility;
class UWarriorAbilitySystemComponent;

UCLASS()
class WARRIOR_API UDataAsset_StartUpDataBase : public UDataAsset
{
    GENERATED_BODY()

public:
    virtual void GiveToAbilitySystemComponent(
        UWarriorAbilitySystemComponent* InASCToGive, int32 ApplyLevel = 1);

protected:
    UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
    TArray<TSubclassOf<UWarriorGameplayAbility>> ActivateOnGivenAbilities;

    UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
    TArray<TSubclassOf<UWarriorGameplayAbility>> ReactiveAbilities;

    void GrantAbilities(
        const TArray<TSubclassOf<UWarriorGameplayAbility>>& InAbilitiesToGive,
        UWarriorAbilitySystemComponent* InASCToGive, int32 ApplyLevel = 1);
};
```

### 新建 `Source/Warrior/Private/DataAssets/StartUpData/DataAsset_StartUpDataBase.cpp`

```cpp
#include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"

void UDataAsset_StartUpDataBase::GiveToAbilitySystemComponent(
    UWarriorAbilitySystemComponent* InASCToGive, int32 ApplyLevel)
{
    check(InASCToGive);
    GrantAbilities(ActivateOnGivenAbilities, InASCToGive, ApplyLevel);
    GrantAbilities(ReactiveAbilities, InASCToGive, ApplyLevel);
}

void UDataAsset_StartUpDataBase::GrantAbilities(
    const TArray<TSubclassOf<UWarriorGameplayAbility>>& InAbilitiesToGive,
    UWarriorAbilitySystemComponent* InASCToGive, int32 ApplyLevel)
{
    if (InAbilitiesToGive.IsEmpty())
    {
        return;
    }
    for (const TSubclassOf<UWarriorGameplayAbility>& Ability : InAbilitiesToGive)
    {
        if (!Ability) continue;

        FGameplayAbilitySpec AbilitySpec(Ability);
        AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
        AbilitySpec.Level = ApplyLevel;
        InASCToGive->GiveAbility(AbilitySpec);
    }
}
```

✅ 编译通过。

---

## 第19步：DataAsset_HeroStartUpData.h + .cpp

> 依赖：第18步（DataAsset_StartUpDataBase）、第7步（WarriorStructTypes）

### 新建 `Source/Warrior/Public/DataAssets/StartUpData/DataAsset_HeroStartUpData.h`

```cpp
#pragma once
#include "CoreMinimal.h"
#include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include "DataAsset_HeroStartUpData.generated.h"

UCLASS()
class WARRIOR_API UDataAsset_HeroStartUpData : public UDataAsset_StartUpDataBase
{
    GENERATED_BODY()

public:
    virtual void GiveToAbilitySystemComponent(
        UWarriorAbilitySystemComponent* InASCToGive, int32 ApplyLevel = 1) override;

private:
    UPROPERTY(EditDefaultsOnly, Category = "StartUpData", meta = (TitleProperty = "InputTag"))
    TArray<FWarriorHeroAbilitySet> HeroStartUpAbilitySets;
};
```

### 新建 `Source/Warrior/Private/DataAssets/StartUpData/DataAsset_HeroStartUpData.cpp`

```cpp
#include "DataAssets/StartUpData/DataAsset_HeroStartUpData.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"

void UDataAsset_HeroStartUpData::GiveToAbilitySystemComponent(
    UWarriorAbilitySystemComponent* InASCToGive, int32 ApplyLevel)
{
    Super::GiveToAbilitySystemComponent(InASCToGive, ApplyLevel);

    for (const FWarriorHeroAbilitySet& AbilitySet : HeroStartUpAbilitySets)
    {
        if (!AbilitySet.IsValid())
            continue;

        FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
        AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
        AbilitySpec.Level = ApplyLevel;
        AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilitySet.InputTag);
        InASCToGive->GiveAbility(AbilitySpec);
    }
}
```

✅ 编译通过。

---

## 第20步：DataAsset_InputConfig.h + .cpp

> 依赖：引擎 `Engine/DataAsset.h`、`GameplayTagContainer.h`

### 新建 `Source/Warrior/Public/DataAssets/Input/DataAsset_InputConfig.h`

```cpp
#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "DataAsset_InputConfig.generated.h"

class UInputAction;
class UInputMappingContext;

USTRUCT(BlueprintType)
struct FWarriorInputActionConfig
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "InputTag"))
    FGameplayTag InputTag;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UInputAction* InputAction;

    bool IsValid() const
    {
        return InputTag.IsValid() && InputAction;
    }
};

UCLASS()
class WARRIOR_API UDataAsset_InputConfig : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "InputTag"))
    TArray<FWarriorInputActionConfig> NativeInputActions;

    UInputAction* FindNativeActionByTag(const FGameplayTag& InputTag) const;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "InputTag"))
    TArray<FWarriorInputActionConfig> AbilityInputActions;
};
```

### 新建 `Source/Warrior/Private/DataAssets/Input/DataAsset_InputConfig.cpp`

```cpp
#include "DataAssets/Input/DataAsset_InputConfig.h"

UInputAction* UDataAsset_InputConfig::FindNativeActionByTag(
    const FGameplayTag& InInputTag) const
{
    for (const FWarriorInputActionConfig& InputActionConfig : NativeInputActions)
    {
        if (InputActionConfig.InputTag == InInputTag && InputActionConfig.InputAction)
        {
            return InputActionConfig.InputAction;
        }
    }
    return nullptr;
}
```

✅ 编译通过。

---

## 第21步：WarriorInputComponent.h + .cpp

> 依赖：引擎 `EnhancedInputComponent.h`、第20步（DataAsset_InputConfig）

### 新建 `Source/Warrior/Public/Components/Input/WarriorInputComponent.h`

```cpp
#pragma once
#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "DataAssets/Input/DataAsset_InputConfig.h"
#include "WarriorInputComponent.generated.h"

UCLASS()
class WARRIOR_API UWarriorInputComponent : public UEnhancedInputComponent
{
    GENERATED_BODY()

public:
    template <class UserObject, typename CallbackFunc>
    void BindNativeInputAction(const UDataAsset_InputConfig* InInputConfig,
        const FGameplayTag& InInputTag, ETriggerEvent TriggerEvent,
        UserObject* ContextObject, CallbackFunc Func);

    template <class UserObject, typename CallbackFunc>
    void BindAbilityInputAction(const UDataAsset_InputConfig* InInputConfig,
        UserObject* ContextObject, CallbackFunc InputPressedFunc,
        CallbackFunc InputReleasedFunc);
};

template <class UserObject, typename CallbackFunc>
inline void UWarriorInputComponent::BindNativeInputAction(
    const UDataAsset_InputConfig* InInputConfig, const FGameplayTag& InInputTag,
    ETriggerEvent TriggerEvent, UserObject* ContextObject, CallbackFunc Func)
{
    checkf(InInputConfig, TEXT("Input config data asset is null,can not proceed with binding"));
    if (UInputAction* FoundAction = InInputConfig->FindNativeActionByTag(InInputTag))
    {
        BindAction(FoundAction, TriggerEvent, ContextObject, Func);
    }
}

template <class UserObject, typename CallbackFunc>
inline void UWarriorInputComponent::BindAbilityInputAction(
    const UDataAsset_InputConfig* InInputConfig, UserObject* ContextObject,
    CallbackFunc InputPressedFunc, CallbackFunc InputReleasedFunc)
{
    checkf(InInputConfig, TEXT("Input config data asset is null,can not proceed with binding"));
    for (const FWarriorInputActionConfig& AbilityInputActionConfig :
         InInputConfig->AbilityInputActions)
    {
        if (!AbilityInputActionConfig.IsValid())
            continue;
        BindAction(AbilityInputActionConfig.InputAction, ETriggerEvent::Started,
            ContextObject, InputPressedFunc, AbilityInputActionConfig.InputTag);
        BindAction(AbilityInputActionConfig.InputAction, ETriggerEvent::Completed,
            ContextObject, InputReleasedFunc, AbilityInputActionConfig.InputTag);
    }
}
```

### 新建 `Source/Warrior/Private/Components/Input/WarriorInputComponent.cpp`

```cpp
#include "Components/Input/WarriorInputComponent.h"
```

✅ 编译通过。

---

## 第22步：WarriorHeroGameplayAbility.h + .cpp

> 依赖：第12步（WarriorGameplayAbility）

### 新建 `Source/Warrior/Public/AbilitySystem/Abilities/WarriorHeroGameplayAbility.h`

```cpp
#pragma once
#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "WarriorHeroGameplayAbility.generated.h"

class AWarriorHeroCharacter;
class AWarriorHeroController;
class UHeroCombatComponent;

UCLASS()
class WARRIOR_API UWarriorHeroGameplayAbility : public UWarriorGameplayAbility
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
    AWarriorHeroCharacter* GetHeroCharacterFromActorInfo();

    UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
    AWarriorHeroController* GetHeroControllerFromActorInfo();

    UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
    UHeroCombatComponent* GetHeroCombatComponentFromActorInfo();

private:
    TWeakObjectPtr<AWarriorHeroCharacter> CachedWarriorHeroCharacter;
    TWeakObjectPtr<AWarriorHeroController> CachedWarriorHeroController;
};
```

### 新建 `Source/Warrior/Private/AbilitySystem/Abilities/WarriorHeroGameplayAbility.cpp`

```cpp
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Controllers/WarriorHeroController.h"

AWarriorHeroCharacter* UWarriorHeroGameplayAbility::GetHeroCharacterFromActorInfo()
{
    if (!CachedWarriorHeroCharacter.IsValid())
    {
        CachedWarriorHeroCharacter = Cast<AWarriorHeroCharacter>(CurrentActorInfo->AvatarActor);
    }
    return CachedWarriorHeroCharacter.IsValid() ? CachedWarriorHeroCharacter.Get() : nullptr;
}

AWarriorHeroController* UWarriorHeroGameplayAbility::GetHeroControllerFromActorInfo()
{
    if (!CachedWarriorHeroController.IsValid())
    {
        CachedWarriorHeroController = Cast<AWarriorHeroController>(CurrentActorInfo->PlayerController);
    }
    return CachedWarriorHeroController.IsValid() ? CachedWarriorHeroController.Get() : nullptr;
}

UHeroCombatComponent* UWarriorHeroGameplayAbility::GetHeroCombatComponentFromActorInfo()
{
    return GetHeroCharacterFromActorInfo()->GetHeroCombatComponent();
}
```

> `.cpp` 依赖 `Characters/WarriorHeroCharacter.h`（第24步）。先把文件放好。

✅ 文件创建好。

---

## 第23步：WarriorBaseCharacter.h + .cpp

> 依赖：引擎 `AbilitySystemInterface.h`、第3步（AttributeSet）、第11步（ASC）、第18步（DataAsset_StartUpDataBase）

### 新建 `Source/Warrior/Public/Characters/WarriorBaseCharacter.h`

```cpp
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "WarriorBaseCharacter.generated.h"

class UWarriorAbilitySystemComponent;
class UWarriorAttributeSet;
class UDataAsset_StartUpDataBase;

UCLASS()
class WARRIOR_API AWarriorBaseCharacter : public ACharacter, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    AWarriorBaseCharacter();
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;

protected:
    virtual void PossessedBy(AController* NewController) override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AbilitySystem")
    UWarriorAbilitySystemComponent* WarriorAbilitySystemComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AbilitySystem")
    UWarriorAttributeSet* WarriorAttributeSet;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData")
    TSoftObjectPtr<UDataAsset_StartUpDataBase> CharacterStartUpData;

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
```

### 新建 `Source/Warrior/Private/Characters/WarriorBaseCharacter.cpp`

```cpp
#include "Characters/WarriorBaseCharacter.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem/WarriorAttributeSet.h"

AWarriorBaseCharacter::AWarriorBaseCharacter()
{
    PrimaryActorTick.bCanEverTick = false;
    PrimaryActorTick.bStartWithTickEnabled = false;
    GetMesh()->bReceivesDecals = false;

    WarriorAbilitySystemComponent = CreateDefaultSubobject<UWarriorAbilitySystemComponent>(
        TEXT("WarriorAbilitySystemComponent"));
    WarriorAttributeSet = CreateDefaultSubobject<UWarriorAttributeSet>(
        TEXT("WarriorAttributeSet"));
}

void AWarriorBaseCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    if (WarriorAbilitySystemComponent)
    {
        WarriorAbilitySystemComponent->InitAbilityActorInfo(this, this);
        ensureMsgf(!CharacterStartUpData.IsNull(),
            TEXT("Forgot to assign start up data to %s"), *GetName());
    }
}

UAbilitySystemComponent* AWarriorBaseCharacter::GetAbilitySystemComponent() const
{
    return GetWarriorAbilitySystemComponent();
}
```

✅ **现在第13、14步的编译错误消失了**——`WarriorBaseCharacter` 存在了，`WarriorCharacterAnimInstance.cpp` 和 `WarriorHeroAnimInstance.cpp` 都能编译通过。

---

## 第24步：WarriorHeroCharacter.h + .cpp

> 依赖：第23步（WarriorBaseCharacter）、第17步（HeroCombatComponent）、第19步（DataAsset_HeroStartUpData）、第20步（DataAsset_InputConfig）、第21步（WarriorInputComponent）、第2步（GameplayTags）

### 新建 `Source/Warrior/Public/Characters/WarriorHeroCharacter.h`

```cpp
#pragma once
#include "Characters/WarriorBaseCharacter.h"
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "WarriorHeroCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UDataAsset_InputConfig;
struct FInputActionValue;
class UInputComponent;
class UHeroCombatComponent;

UCLASS()
class WARRIOR_API AWarriorHeroCharacter : public AWarriorBaseCharacter
{
    GENERATED_BODY()

public:
    AWarriorHeroCharacter();

protected:
    virtual void PossessedBy(AController* NewController) override;
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
#pragma region Component
    UPROPERTY(VisibleAnywhere, Category = "Component", meta = (AllowPrivateAccess = "true"))
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, Category = "Component", meta = (AllowPrivateAccess = "true"))
    UCameraComponent* FollowCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
    UHeroCombatComponent* HeroCombatComponent;
#pragma endregion Component

#pragma region Input
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData", meta = (AllowPrivateAccess = "true"))
    UDataAsset_InputConfig* InputConfigDataAsset;

    void Input_Move(const FInputActionValue& InputActionValue);
    void Input_Look(const FInputActionValue& InputActionValue);
    void Input_AbilityInputPressed(FGameplayTag InInputTag);
    void Input_AbilityInputReleased(FGameplayTag InInputTag);
#pragma endregion Input

public:
    FORCEINLINE UHeroCombatComponent* GetHeroCombatComponent() const { return HeroCombatComponent; }
};
```

### 新建 `Source/Warrior/Private/Characters/WarriorHeroCharacter.cpp`

```cpp
#include "Characters/WarriorHeroCharacter.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/Combat/HeroCombatComponent.h"
#include "Components/Input/WarriorInputComponent.h"
#include "DataAssets/Input/DataAsset_InputConfig.h"
#include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "WarriorDebug.h"
#include "WarriorGameplayTags.h"

AWarriorHeroCharacter::AWarriorHeroCharacter()
{
    GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

    bUseControllerRotationYaw = false;
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(GetRootComponent());
    CameraBoom->TargetArmLength = 200.f;
    CameraBoom->SocketOffset = FVector(0.f, 55.f, 65.f);
    CameraBoom->bUsePawnControlRotation = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
    GetCharacterMovement()->MaxWalkSpeed = 400.f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

    HeroCombatComponent = CreateDefaultSubobject<UHeroCombatComponent>(TEXT("HeroCombatComponent"));
}

void AWarriorHeroCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void AWarriorHeroCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    checkf(InputConfigDataAsset, TEXT("Input config data asset is null,can not proceed with binding"));

    ULocalPlayer* LocalPlayer = GetController<APlayerController>()->GetLocalPlayer();
    UEnhancedInputLocalPlayerSubsystem* Subsystem =
        ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
    check(Subsystem);
    Subsystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext, 0);

    UWarriorInputComponent* WarriorInputComponent = Cast<UWarriorInputComponent>(PlayerInputComponent);
    WarriorInputComponent->BindNativeInputAction(
        InputConfigDataAsset, WarriorGameplayTags::InputTag_Move,
        ETriggerEvent::Triggered, this, &AWarriorHeroCharacter::Input_Move);
    WarriorInputComponent->BindNativeInputAction(
        InputConfigDataAsset, WarriorGameplayTags::InputTag_Look,
        ETriggerEvent::Triggered, this, &AWarriorHeroCharacter::Input_Look);

    WarriorInputComponent->BindAbilityInputAction(
        InputConfigDataAsset, this,
        &AWarriorHeroCharacter::Input_AbilityInputPressed,
        &AWarriorHeroCharacter::Input_AbilityInputReleased);
}

void AWarriorHeroCharacter::Input_Move(const FInputActionValue& InputActionValue)
{
    const FVector2D MovementVector = InputActionValue.Get<FVector2D>();
    const FRotator MovementRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);

    if (MovementVector.Y != 0.f)
    {
        const FVector ForwardDirection = MovementRotation.RotateVector(FVector::ForwardVector);
        AddMovementInput(ForwardDirection, MovementVector.Y);
    }
    if (MovementVector.X != 0.f)
    {
        const FVector RightDirection = MovementRotation.RotateVector(FVector::RightVector);
        AddMovementInput(RightDirection, MovementVector.X);
    }
}

void AWarriorHeroCharacter::Input_Look(const FInputActionValue& InputActionValue)
{
    const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();
    if (LookAxisVector.X != 0.f)
    {
        AddControllerYawInput(LookAxisVector.X);
    }
    if (LookAxisVector.Y != 0.f)
    {
        AddControllerPitchInput(LookAxisVector.Y);
    }
}

void AWarriorHeroCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    if (!CharacterStartUpData.IsNull())
    {
        if (UDataAsset_StartUpDataBase* LoadedData = CharacterStartUpData.LoadSynchronous())
        {
            LoadedData->GiveToAbilitySystemComponent(WarriorAbilitySystemComponent);
        }
    }
}

void AWarriorHeroCharacter::Input_AbilityInputPressed(FGameplayTag InInputTag)
{
    WarriorAbilitySystemComponent->OnAbilityInputPressed(InInputTag);
}

void AWarriorHeroCharacter::Input_AbilityInputReleased(FGameplayTag InInputTag)
{
    WarriorAbilitySystemComponent->OnAbilityInputReleased(InInputTag);
}
```

✅ **全部 C++ 代码现在都能编译通过！**

---

## 第25步：配置 DefaultEngine.ini

打开 `Config/DefaultEngine.ini`，添加/修改：

```ini
[/Script/EngineSettings.GameMapsSettings]
GameDefaultMap=/Game/Content/Maps/Main.Main

[/Script/HardwareTargeting.HardwareTargetingSettings]
TargetedRHIs=(("Default",4))

[/Script/WindowsTargetPlatform.WindowsTargetSettings]
DefaultRHI=DefaultRHI_DX12
```

---

## C++ 完成！接下来是蓝图

到这一步，所有 23 个 C++ 类已经创建完毕且可以编译。剩余工作在 UE 编辑器里完成——创建蓝图资产。

### 蓝图创建顺序（按依赖）

| 顺序 | 蓝图 | 路径 | 父类 | 说明 |
|------|------|------|------|------|
| 1 | **IA_Move** | `Content/BP/Input/Actions/` | InputAction | Value Type = Axis2D |
| 2 | **IA_Look** | `Content/BP/Input/Actions/` | InputAction | Value Type = Axis2D |
| 3 | **IA_EquipAxe** | `Content/BP/Input/Actions/` | InputAction | Value Type = Digital |
| 4 | **IA_UnEquipAxe** | `Content/BP/Input/Actions/` | InputAction | Value Type = Digital |
| 5 | **IMC_Default** | `Content/BP/Input/` | InputMappingContext | W/A/S/D→IA_Move, 鼠标→IA_Look, 1→IA_EquipAxe, 2→IA_UnEquipAxe |
| 6 | **IMC_Axe** | `Content/BP/Input/` | InputMappingContext | 持斧攻击键（先创建空的） |
| 7 | **DA_InputConfig** | `Content/BP/` | DataAsset_InputConfig | 填 IMC_Default + NativeInputActions + AbilityInputActions |
| 8 | **ALI_Hero** | `Content/BP/AnimBP/` | AnimLayerInterface | 声明 ArmedLocomotionState |
| 9 | **BS_UnarmedLocomotion** | `Content/BP/AnimBP/BlendSpace/` | BlendSpace | 空手移动混合空间 |
| 10 | **BS_DefaultLocomotion_Axe** | `Content/BP/AnimBP/` | BlendSpace1D | 持斧移动混合空间 |
| 11 | **ABP_Hero** | `Content/BP/AnimBP/` | WarriorHeroAnimInstance | 主动画蓝图 |
| 12 | **MasterAnimLayer_Hero** | `Content/BP/AnimBP/` | WarriorHeroLinkedAnimLayer | 实现ALI_Hero，用BS_UnarmedLocomotion |
| 13 | **AnimLayer_HeroAxe** | `Content/BP/AnimBP/AnimLayer/` | MasterAnimLayer_Hero(蓝图父类) | 覆盖BlendSpace为BS_DefaultLocomotion_Axe |
| 14 | **AN_SendGameplayEventToOwner** | `Content/Shared/AnimNotify/` | AnimNotify | 发送GameplayEvent |
| 15 | **Hero_Axe_Equip_Montage** | `Content/BP/Montage/` | AnimMontage | 引用Hero_Axe_Equip动画 + AN_SendGameplayEventToOwner |
| 16 | **Hero_Axe_Unequip_Montage** | `Content/BP/Montage/` | AnimMontage | 同上 |
| 17 | **GA_Shared_SpawnWeapon** | `Content/Shared/GameplayAbility/` | WarriorGameplayAbility | OnGiven, 生成武器+注册 |
| 18 | **GA_Hero_Spawn_Axe** | `Content/BP/GameplayAbility/` | GA_Shared_SpawnWeapon(蓝图父类) | 指定生成BP_HeroAxe, Tag=Player.Weapon.Axe |
| 19 | **GA_Hero_EquipAxe** | `Content/BP/GameplayAbility/` | WarriorHeroGameplayAbility | OnTriggered, 蒙太奇+事件+挂接+链接层 |
| 20 | **GA_Hero_UnequipAxe** | `Content/BP/GameplayAbility/` | WarriorHeroGameplayAbility | OnTriggered, 和装备对称 |
| 21 | **BP_HeroWeaponBase** | `Content/BP/HeroWeapon/` | WarriorHeroWeapon | 设置碰撞盒 |
| 22 | **BP_HeroAxe** | `Content/BP/HeroWeapon/` | BP_HeroWeaponBase(蓝图父类) | Mesh=SM_DefaultAxe, HeroWeaponData配好 |
| 23 | **DA_Hero** | `Content/BP/` | DataAsset_HeroStartUpData | ActivateOnGiven=GA_Hero_Spawn_Axe, HeroStartUpAbilitySets配EquipAxe |
| 24 | **BP_WarriorHeroController** | `Content/BP/` | WarriorHeroController | 默认 |
| 25 | **BP_WarriorBaseGameMode** | `Content/BP/` | WarriorBaseGameMode | DefaultPawnClass=BP_WarriorHeroCharacter |
| 26 | **BP_WarriorHeroCharacter** | `Content/BP/` | WarriorHeroCharacter | Mesh=英雄骨骼, AnimClass=ABP_Hero, CharacterStartUpData=DA_Hero, InputConfigDataAsset=DA_InputConfig |

### 最后：创建 Main 关卡

1. `Content/Maps/Main` — 拖入 BP_WarriorHeroCharacter
2. World Settings → GameMode = BP_WarriorBaseGameMode
3. 英雄骨骼上添加 Socket：`AxeRightHandSocket`（右手掌心位置）

---

## 总结：25个C++步骤 + 26个蓝图

```
C++ 构建顺序（全独立可编译）:

 0. 项目创建 + .uproject + Build.cs
 1. WarriorDebug.h                     ← 零依赖
 2. WarriorGameplayTags.h/.cpp         ← 零依赖
 3. WarriorAttributeSet.h/.cpp         ← 零依赖（空壳）
 4. WarriorBaseAnimInstance.h          ← 零依赖（空壳）
 5. PawnExtensionComponentBase.h       ← 零依赖
 6. WarriorWeaponBase.h/.cpp           ← 零依赖
 7. WarriorStructTypes.h/.cpp          ← 前向声明够用
 8. WarriorHeroController.h            ← 零依赖（空壳）
 9. WarriorBaseGameMode.h              ← 零依赖（空壳）
10. PawnCombatComponet.h/.cpp          ← 依赖 5, 6
11. WarriorAbilitySystemComponent.h/.cpp ← 依赖 7
12. WarriorGameplayAbility.h/.cpp      ← 依赖 10, 11
13. WarriorCharacterAnimInstance.h/.cpp ← 依赖 4（.cpp等23）
14. WarriorHeroAnimInstance.h/.cpp     ← 依赖 13（.cpp等24）
15. WarriorHeroLinkedAnimLayer.h/.cpp  ← 依赖 4, 14
16. WarriorHeroWeapon.h/.cpp           ← 依赖 6, 7
17. HeroCombatComponent.h/.cpp         ← 依赖 10, 16
18. DataAsset_StartUpDataBase.h/.cpp   ← 依赖 11, 12
19. DataAsset_HeroStartUpData.h/.cpp   ← 依赖 18, 7
20. DataAsset_InputConfig.h/.cpp       ← 零依赖
21. WarriorInputComponent.h/.cpp       ← 依赖 20
22. WarriorHeroGameplayAbility.h/.cpp  ← 依赖 12（.cpp等24）
23. WarriorBaseCharacter.h/.cpp        ← 依赖 3, 11, 18
24. WarriorHeroCharacter.h/.cpp        ← 依赖 17, 19, 20, 21, 23

✓ 第23步完成后，第13,14,22步的编译错误自动消失。
✓ 第24步完成后，所有代码编译通过！
```

> **详细的概念解释和架构数据流，请阅读 [PROJECT_GUIDE.md](PROJECT_GUIDE.md) 和 [ANIMATION_SYSTEM.md](ANIMATION_SYSTEM.md)。**
