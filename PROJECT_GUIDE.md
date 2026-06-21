# Warrior 项目完全指南

> 读完这篇，你就能理解项目的每一个 C++ 类、每一个蓝图、以及它们之间是怎么配合的。按「从启动到按1键」的时间顺序组织。

---

## 目录

1. [先知道这五个概念](#一先知道这五个概念)
2. [项目启动：谁创建了谁](#二项目启动谁创建了谁)
3. [游戏开始：技能怎么来的](#三游戏开始技能怎么来的)
4. [斧头是怎么生成的](#四斧头是怎么生成的)
5. [每帧：动画怎么动的](#五每帧动画怎么动的)
6. [按1键：装备斧头全流程](#六按1键装备斧头全流程)
7. [按其他键：卸载斧头](#七按其他键卸载斧头)
8. [C++ 类逐个解释](#八c-类逐个解释)
9. [蓝图逐个解释](#九蓝图逐个解释)
10. [数据怎么流动（速查表）](#十数据怎么流动速查表)

---

## 一、先知道这五个概念

### 概念1：GameplayAbilitySystem (GAS)

GAS 是 UE 官方做的一套"技能系统"。它有四个核心概念：

```
AbilitySystemComponent (ASC)     — 技能的"管理器"，挂在角色身上
GameplayAbility                   — 一个"技能"，比如"装备斧头"就是一个技能
GameplayTag                       — 一个标签，比如 "Player.Weapon.Axe"
GameplayEffect                    — 一个"效果"，比如加攻击力 buff
AttributeSet                      — 角色的"属性"，比如血量、蓝量
```

**这个项目只用了前三个**（ASC、Ability、Tag），后两个还没用到。

### 概念2：GameplayTag

就是一个用点分层的字符串标签，比如 `Player.Weapon.Axe`。它不是枚举，不需要提前声明所有可能的值——想加个 `Player.Weapon.Sword` 随时可以加。

**Tag 在这个项目里无处不在**——按键绑定用 Tag、技能查找用 Tag、武器查找用 Tag、动画事件用 Tag。理解 Tag 就理解了项目的一半。

### 概念3：技能激活的两种方式

```
OnGiven      — 技能被授予给角色时立刻自动激活
OnTriggered  — 技能被授予后等着，按了键才激活
```

`GA_Hero_Spawn_Axe` 是 `OnGiven`（游戏开始自动生成斧头）。
`GA_Hero_EquipAxe` 是 `OnTriggered`（按"1"才激活）。

### 概念4：链接动画层 (Linked Anim Layer)

UE5 的一种动画技术。让你可以在**不修改主动画蓝图**的情况下，替换掉其中一部分动画。

```
ABP_Hero（主动画蓝图）
  ├─ 上半身动画 ← 这部分不变
  ├─ 下半身动画 ← 这部分也不变
  └─ ArmedLocomotionState ← "插槽"，可以被链接层替换
         │
         ├─ 空手时 → MasterAnimLayer_Hero 提供空手动画
         └─ 持斧时 → AnimLayer_HeroAxe 提供持斧动画
```

### 概念5：DataAsset

一种特殊的 UE 资源——它只存数据，不存逻辑。在编辑器的 DataAsset 面板里填好数字/Tag/引用，运行时读取。

这个项目有两个关键的 DataAsset：

```
DA_Hero       — "这个英雄有哪些技能？哪个技能绑定哪个按键？"
DA_InputConfig — "哪个按键动作对应哪个 GameplayTag？"
```

---

## 二、项目启动：谁创建了谁

你按下 Play 按钮后，引擎做的事：

```
1. 关卡里有一个 BP_WarriorHeroCharacter（你拖进去的）
       │
2. 调用 AWarriorBaseCharacter 的构造函数
       │
3. 创建了两个子对象：
       ├─ WarriorAbilitySystemComponent  ← GAS 管理器
       └─ WarriorAttributeSet            ← 属性（还没用上）
       │
4. 调用 AWarriorHeroCharacter 的构造函数
       │
5. 创建了三个子对象：
       ├─ CameraBoom          ← 弹簧臂（让相机可以拉远拉近）
       ├─ FollowCamera        ← 相机
       └─ HeroCombatComponent ← 战斗组件（管理武器）
       │
6. 角色站在场景里等着
       │
7. Controller 接管这个角色 → 调用 PossessedBy()
       │
8. AWarriorBaseCharacter::PossessedBy()
       ├─ ASC->InitAbilityActorInfo(this, this)
       │  （告诉 GAS："这个 ASC 属于这个角色"）
       │
       └─ 检查 CharacterStartUpData 有没有设置
          （指向 DA_Hero 数据资产，蓝图中配好的）
       │
9. AWarriorHeroCharacter::PossessedBy()
       ├─ 加载 DA_Hero
       └─ DA_Hero->GiveToAbilitySystemComponent(ASC)
          （把技能列表灌给 GAS）
```

**此时：**角色站在场景里，ASC 里已经有了技能，但斧头装备技能还没激活，动画还是空手状态。

---

## 三、游戏开始：技能怎么来的

### 数据资产 DA_Hero 长什么样（逻辑上）

```
DA_Hero:
  ActivateOnGivenAbilities:      ← 这些技能一给就自动激活
    [0] GA_Hero_Spawn_Axe        ← "生成一把斧头"
  
  ReactiveAbilities:             ← 这些是被动技能（还没用到）
    []                           ← 空的
  
  HeroStartUpAbilitySets:        ← 这些技能绑定到按键
    [0] {
      InputTag: InputTag.EquipAxe           ← 按"1"键
      AbilityToGrant: GA_Hero_EquipAxe      ← 激活这个技能
    }
```

### DA_Hero 里发生了什么（C++ 代码）

```cpp
// DataAsset_HeroStartUpData.cpp
void UDataAsset_HeroStartUpData::GiveToAbilitySystemComponent(ASC)
{
    // 第一步：让父类处理 ActivateOnGivenAbilities
    Super::GiveToAbilitySystemComponent(ASC);
    // 这一步会用"OnGiven"方式给所有 ActivateOnGivenAbilities 里的技能
    // GA_Hero_Spawn_Axe 会立刻激活 → 生成斧头Actor

    // 第二步：处理 HeroStartUpAbilitySets
    for (英雄技能列表里的每一项) {
        创建一个技能;
        给这个技能贴上 InputTag;  // 比如 "InputTag.EquipAxe"
        ASC->GiveAbility(技能);   // 技能加入ASC，等按键触发
    }
}
```

### 顺便说一句：按键映射怎么配的

```
IMC_Default (输入映射上下文)
  ├─ 键位 "1"  → IA_EquipAxe       ← 按1触发这个输入动作
  ├─ 键位 W/A/S/D → IA_Move         ← 移动
  └─ 鼠标 → IA_Look                  ← 视角

DA_InputConfig (输入配置数据)
  ├─ NativeInputActions:            ← 直接调C++函数的
  │    [0] InputTag.Move → IA_Move
  │    [1] InputTag.Look → IA_Look
  │
  └─ AbilityInputActions:           ← 走 GAS 技能系统的
       [0] InputTag.EquipAxe → IA_EquipAxe
```

---

## 四、斧头是怎么生成的

### GA_Hero_Spawn_Axe 激活后做的事

```
1. 在场景里创建一个 BP_HeroAxe Actor
      │
2. 调用 PawnCombatComponent->RegisterSpawnedWeapon(
        "Player.Weapon.Axe",    ← 用这个 Tag 登记
        斧头Actor,              ← 存起来
        false                   ← "不是当前装备的武器"
   )
      │
3. 斧头被存入 CharacterCarriedWeaponMap
   这个 Map 现在是:
   { "Player.Weapon.Axe" → 斧头Actor的指针 }
      │
4. 斧头Actor存在了，但：
   - 没有挂到角色手上
   - 没有显示在场景里
   - CurrentEquippedWeaponTag 还是空的
```

### BP_HeroAxe 自己带了什么数据

```
BP_HeroAxe:
  ├─ WeaponMesh = SM_DefaultAxe（斧头3D模型）
  ├─ WeaponCollisionBox（碰撞检测盒）
  └─ HeroWeaponData（FWarriorHeroWeaponTypes 结构体）:
       ├─ WeaponAnimLayerToLink = AnimLayer_HeroAxe
       │    （装备斧头时链接这个动画层）
       └─ WeaponInputMappingContext = IMC_Axe
            （装备斧头时激活这个输入映射）
```

---

## 五、每帧：动画怎么动的

### 第一步：计算数据（C++ 里）

```
每帧执行：

UWarriorCharacterAnimInstance::NativeThreadSafeUpdateAnimation():
  ┌─────────────────────────────────────────────┐
  │ GroundSpeed = 角色速度的2D长度               │
  │  → 站着不动 = 0                              │
  │  → 走路     = 100-200                        │
  │  → 跑步     = 300-400                        │
  │                                              │
  │ bHasAcceleration = 有没有按移动键             │
  │  → 按了W/A/S/D = true                        │
  │  → 松开        = false                       │
  └─────────────────────────────────────────────┘

UWarriorHeroAnimInstance::NativeUpdateAnimation():
  ┌─────────────────────────────────────────────┐
  │ if (正在移动):                               │
  │     IdleElapsedTime = 0                      │
  │     bShouldEnterRelaxState = false           │
  │ else:                                        │
  │     IdleElapsedTime += 这一帧的秒数           │
  │     if (IdleElapsedTime >= 5秒):             │
  │         bShouldEnterRelaxState = true         │
  └─────────────────────────────────────────────┘
```

### 第二步：选的什么动画（动画蓝图里）

```
ABP_Hero 读到 GroundSpeed:

  GroundSpeed ≈ 0   → 播放 Idle 动画
  GroundSpeed 1-200 → 播放 Walk 动画
  GroundSpeed >200  → 播放 Jog 动画

ABP_Hero 读到 bShouldEnterRelaxState:

  true  → 播放 Relax 动画（伸懒腰之类的）

ABP_Hero 的 ArmedLocomotionState 槽位:

  空手 → MasterAnimLayer_Hero 提供动画（Hero_Unarmed_Idle / Walk / Jog）
  持斧 → AnimLayer_HeroAxe 提供动画（Hero_Idle_Axe / Hero_Axe_Walk / Hero_Axe_Jog）
```

### 链接动画层怎么知道用哪个动画

```
AnimLayer_HeroAxe 有一个 BlendSpace1D: BS_DefaultLocomotion_Axe

BS_DefaultLocomotion_Axe 的横轴是 GroundSpeed:
  轴值 0   → Hero_Idle_Axe
  轴值 150 → Hero_Axe_Walk_Fwd
  轴值 400 → Hero_Axe_JogFwd_Loop
```

---

## 六、按1键：装备斧头全流程

### 按键 → 找到技能

```
1. 玩家按下键盘 "1"
      │
2. Enhanced Input 系统匹配:
       IMC_Default 里 "1" → IA_EquipAxe
      │
3. WarriorInputComponent 收到 IA_EquipAxe:
       从 DA_InputConfig 的 AbilityInputActions 里找到:
       IA_EquipAxe → InputTag.EquipAxe
      │
4. 调用 Input_AbilityInputPressed("InputTag.EquipAxe")
      │
5. AWarriorHeroCharacter::Input_AbilityInputPressed():
       传给 ASC->OnAbilityInputPressed("InputTag.EquipAxe")
      │
6. ASC 遍历所有已授予的技能:
       找到 DynamicSpecSourceTags 里有 "InputTag.EquipAxe" 的那个
       → 是 GA_Hero_EquipAxe!
      │
7. TryActivateAbility(GA_Hero_EquipAxe)
```

### 技能激活后做什么

```
GA_Hero_EquipAxe::ActivateAbility():

  ┌──────────────────────────────────────────────────┐
  │ 步骤①: 播放蒙太奇                                │
  │ PlayMontageAndWait(Hero_Axe_Equip_Montage)        │
  │ 角色开始播"拿出斧头"的动画                         │
  └──────────────────────────────────────────────────┘
         │
         ▼
  ┌──────────────────────────────────────────────────┐
  │ 步骤②: 等待动画事件                              │
  │ WaitGameplayEvent("Player.Event.Equip.Axe")       │
  │ 等着蒙太奇里的 AnimNotify 触发                    │
  └──────────────────────────────────────────────────┘
         │
         │   蒙太奇播到某个关键时刻
         │   → AN_SendGameplayEventToOwner 触发
         │   → 发送 GameplayEvent("Player.Event.Equip.Axe")
         │
         ▼
  ┌──────────────────────────────────────────────────┐
  │ 步骤③: 事件回调 —— 做实际的装备工作               │
  │                                                   │
  │ a. 从 CombatComponent 取出斧头                    │
  │    GetHeroCharacterCarriedWeaponByTag(             │
  │        "Player.Weapon.Axe")                       │
  │                                                   │
  │ b. 把斧头物理挂接到右手骨骼上                      │
  │    AttachToComponent(Mesh, "AxeRightHandSocket")  │
  │                                                   │
  │ c. 链接持斧动画层                                 │
  │    LinkAnimClassLayers(AnimLayer_HeroAxe)         │
  │    → 空手动画立刻切换为持斧动画                    │
  │                                                   │
  │ d. 标记当前装备的武器                              │
  │    CurrentEquippedWeaponTag = "Player.Weapon.Axe" │
  │                                                   │
  │ e. 激活持斧输入映射                                │
  │    AddMappingContext(IMC_Axe)                     │
  │    → 现在攻击键可以用了                            │
  └──────────────────────────────────────────────────┘
         │
         ▼
  ┌──────────────────────────────────────────────────┐
  │ 步骤④: 蒙太奇播完 → EndAbility()                 │
  │ 装备技能结束，角色恢复待机状态                      │
  └──────────────────────────────────────────────────┘
```

### 动画切换的关键代码

```
LinkAnimClassLayers(AnimLayer_HeroAxe) 做的事情:

  SkeletalMeshComponent 有一个 LinkedAnimLayers 列表
  调用后 AnimLayer_HeroAxe 被加入这个列表
  ABP_Hero 的 ArmedLocomotionState 现在输出：
    BS_DefaultLocomotion_Axe → 持斧动画
    而不是之前的 MasterAnimLayer_Hero → 空手动画
```

---

## 七、按其他键：卸载斧头

流程和装备完全对称：

```
按键 → IA_UnEquipAxe → GA_Hero_UnequipAxe 激活
  → PlayMontageAndWait(Hero_Axe_Unequip_Montage)
  → WaitGameplayEvent("Player.Event.Unequip.Axe")
  → AnimNotify 触发 → 事件回调:
       ├─ DetachFromComponent()              (从手上取下)
       ├─ CurrentEquippedWeaponTag = 空      (标记空手)
       ├─ UnlinkAnimClassLayers()             (恢复空手动画)
       └─ RemoveMappingContext(IMC_Axe)       (移除持斧按键)
```

---

## 八、C++ 类逐个解释

### 最上层：角色

#### `AWarriorBaseCharacter`（[WarriorBaseCharacter.h](Source/Warrior/Public/Characters/WarriorBaseCharacter.h)）

```
作用：角色基类，Hero 和未来的 Enemy 都继承它

做了什么：
  1. 创建 ASC（技能管理器）
  2. 创建 AttributeSet（属性集，还没用）
  3. PossessedBy() 里调用 ASC->InitAbilityActorInfo()
     — 告诉 GAS 这个 ASC 属于谁
  4. 暴露 CharacterStartUpData 变量给蓝图配置

关键变量：
  WarriorAbilitySystemComponent  — ASC 子对象
  WarriorAttributeSet             — 属性子对象
  CharacterStartUpData            — 指向 DA_Hero 的软引用
```

#### `AWarriorHeroCharacter`（[WarriorHeroCharacter.h](Source/Warrior/Public/Characters/WarriorHeroCharacter.h)）

```
作用：玩家控制的英雄角色

做了什么：
  1. 创建相机（SpringArm + Camera）
  2. 创建 HeroCombatComponent
  3. SetupPlayerInputComponent():
       - 添加 IMC_Default 输入映射
       - 绑定 Move/Look（直接调C++函数）
       - 绑定所有 AbilityInputAction（走 GAS 技能系统）
  4. PossessedBy():
       - 加载 DA_Hero
       - 把技能塞给 ASC

关键变量：
  CameraBoom                    — 弹簧臂组件
  FollowCamera                  — 跟随相机
  HeroCombatComponent           — 战斗组件
  InputConfigDataAsset          — 指向 DA_InputConfig
```

---

### 技能系统

#### `UWarriorAbilitySystemComponent`（[WarriorAbilitySystemComponent.h](Source/Warrior/Public/AbilitySystem/WarriorAbilitySystemComponent.h)）

```
作用：GAS 管理器，角色的"技能插槽"

做了什么：
  OnAbilityInputPressed(Tag):
    遍历所有已授予的技能
    找到 DynamicSpecSourceTags 里贴了指定 Tag 的
    调用 TryActivateAbility() 激活它

  OnAbilityInputReleased(Tag):
    目前是空的（按下去的逻辑够用了）
```

#### `UWarriorGameplayAbility`（[WarriorGameplayAbility.h](Source/Warrior/Public/AbilitySystem/Abilities/WarriorGameplayAbility.h)）

```
作用：所有技能的基类

做了什么：
  OnGiveAbility():
    如果 ActivationPolity == OnGiven → 立刻激活
  
  EndAbility():
    如果 ActivationPolity == OnGiven → 激活一次后删除（用过的技能不再需要）

  提供的工具函数：
    GetPawnCombatComponentFromActorInfo()  — 拿战斗组件
    GetWarriorAbilitySystemComponentFromActorInfo() — 拿ASC

关键变量：
  ActivationPolity  — OnGiven（自动激活）还是 OnTriggered（等按键）
```

#### `UWarriorHeroGameplayAbility`（[WarriorHeroGameplayAbility.h](Source/Warrior/Public/AbilitySystem/Abilities/WarriorHeroGameplayAbility.h)）

```
作用：英雄专属技能的基类，多提供几个工具函数

做了什么：
  GetHeroCharacterFromActorInfo()    — 拿 HeroCharacter
  GetHeroControllerFromActorInfo()   — 拿 HeroController
  GetHeroCombatComponentFromActorInfo() — 拿 CombatComponent

缓存机制：第一次调用时 Cast 并缓存，后续直接返回缓存
```

---

### 动画系统

#### `UWarriorBaseAnimInstance`（[WarriorBaseAnimInstance.h](Source/Warrior/Public/AnimInstances/WarriorBaseAnimInstance.h)）

```
作用：所有 Warrior 动画实例的根类
内容：空的，什么都没加
用途：占位，将来如果有所有角色共享的动画逻辑可以放这里
```

#### `UWarriorCharacterAnimInstance`（[WarriorCharacterAnimInstance.h](Source/Warrior/Public/AnimInstances/WarriorCharacterAnimInstance.h)）

```
作用：所有 Warrior 角色的动画基类

做了什么：
  NativeInitializeAnimation():
    缓存 OwningCharacter 和 OwningMovementComponent

  NativeThreadSafeUpdateAnimation():  ← 每帧执行，后台线程安全
    GroundSpeed = GetVelocity().Size2D()          — 当前移动速度
    bHasAcceleration = GetCurrentAcceleration() > 0 — 是否按下移动键

关键变量（暴露给动画蓝图）：
  GroundSpeed       — float，0=静止，400=全速跑
  bHasAcceleration  — bool，有没有按移动键
```

#### `UWarriorHeroAnimInstance`（[WarriorHeroAnimInstance.h](Source/Warrior/Public/AnimInstances/Hero/WarriorHeroAnimInstance.h)）

```
作用：英雄的动画实例

做了什么：
  NativeUpdateAnimation():  ← 每帧执行，游戏线程
    如果正在移动 → 重置空闲计时器
    如果站着不动 → 累加空闲时间
    空闲超过 5 秒 → bShouldEnterRelaxState = true

关键变量：
  bShouldEnterRelaxState  — bool，是否该播放放松动画
  EnterRelaxStateTime     — float=5，放松前需要空闲的秒数
  IdleElapsedTime         — float，当前已空闲的时间
  OwningHeroCharacter     — 缓存的英雄引用
```

#### `UWarriorHeroLinkedAnimLayer`（[WarriorHeroLinkedAnimLayer.h](Source/Warrior/Public/AnimInstances/Hero/WarriorHeroLinkedAnimLayer.h)）

```
作用：英雄链接动画层的基类

做了什么：
  GetHeroAnimInstance():
    获取角色的 SkeletalMeshComponent
    获取它的主 AnimInstance（即 ABP_Hero 实例）
    Cast 成 UWarriorHeroAnimInstance 返回

  这个函数是链接动画层和主动画实例之间的桥梁：
    链接层通过它读取 GroundSpeed、bShouldEnterRelaxState 等变量
```

---

### 武器与战斗

#### `AWarriorWeaponBase`（[WarriorWeaponBase.h](Source/Warrior/Public/Items/Weapons/WarriorWeaponBase.h)）

```
作用：所有武器的 Actor 基类

做了什么：
  构造函数里创建：
    WeaponMesh        — 武器的 3D 模型（StaticMeshComponent，作为根组件）
    WeaponCollisionBox — 碰撞检测盒（BoxComponent，目前碰撞关闭）

为什么是 Actor 而不是 Component：
  武器需要独立于角色的生命周期。
  生成、挂接、卸载、丢弃——用 Actor 更容易管理。
```

#### `AWarriorHeroWeapon`（[WarriorHeroWeapon.h](Source/Warrior/Public/Items/Weapons/WarriorHeroWeapon.h)）

```
作用：英雄武器的基类

做了什么：
  加了一个 HeroWeaponData 变量

关键变量：
  HeroWeaponData (FWarriorHeroWeaponTypes):
    WeaponAnimLayerToLink       — 装备时链接哪个动画层（TSubclassOf）
    WeaponInputMappingContext   — 装备时激活哪个输入映射
```

#### `UPawnCombatComponet`（[PawnCombatComponet.h](Source/Warrior/Public/Components/Combat/PawnCombatComponet.h)）

```
作用：管理角色携带的所有武器

做了什么：
  RegisterSpawnedWeapon(Tag, Weapon, bEquipped):
    把武器存入 TMap<FGameplayTag, AWarriorWeaponBase*>
    如果 bEquipped=true → 设置为当前装备武器

  GetCharacterCarriedWeaponByTag(Tag):
    从 TMap 里按 Tag 查找武器

  GetCharacterCurrentEquippedWeapon():
    返回 CurrentEquippedWeaponTag 对应的武器

关键变量：
  CharacterCarriedWeaponMap — TMap<FGameplayTag, 武器指针>
  CurrentEquippedWeaponTag  — 当前装备的是哪把武器
```

#### `UHeroCombatComponent`（[HeroCombatComponent.h](Source/Warrior/Public/Components/Combat/HeroCombatComponent.h)）

```
作用：在父类基础上多加一个英雄专用的武器查询函数

做了什么：
  GetHeroCharacterCarriedWeaponByTag(Tag):
    调父类的 GetCharacterCarriedWeaponByTag
    然后 Cast<AWarriorHeroWeapon> 返回
```

---

### 数据资产

#### `UDataAsset_InputConfig`（[DataAsset_InputConfig.h](Source/Warrior/Public/DataAssets/Input/DataAsset_InputConfig.h)）

```
作用：存储输入映射的配置数据

结构：
  DefaultMappingContext    — 默认 IMC（IMC_Default）
  
  NativeInputActions[]:    — 直接调C++的输入
    每一项都有 InputTag + InputAction
    例如：InputTag.Move → IA_Move
  
  AbilityInputActions[]:   — 走GAS技能的输入
    每一项都有 InputTag + InputAction
    例如：InputTag.EquipAxe → IA_EquipAxe

函数：
  FindNativeActionByTag(Tag) → 返回对应的 InputAction
```

#### `UDataAsset_StartUpDataBase`（[DataAsset_StartUpDataBase.h](Source/Warrior/Public/DataAssets/StartUpData/DataAsset_StartUpDataBase.h)）

```
作用：技能启动数据的基类

做了什么：
  GiveToAbilitySystemComponent(ASC):
    → 授予 ActivateOnGivenAbilities（自动激活）
    → 授予 ReactiveAbilities（被动技能）

  GrantAbilities(列表, ASC):
    遍历列表，为每个技能创建 FGameplayAbilitySpec
    调用 ASC->GiveAbility()

关键变量：
  ActivateOnGivenAbilities[] — TSubclassOf<UWarriorGameplayAbility>
  ReactiveAbilities[]         — TSubclassOf<UWarriorGameplayAbility>
```

#### `UDataAsset_HeroStartUpData`（[DataAsset_HeroStartUpData.h](Source/Warrior/Public/DataAssets/StartUpData/DataAsset_HeroStartUpData.h)）

```
作用：英雄专用的启动数据

做了什么：
  GiveToAbilitySystemComponent(ASC):
    ① 调父类 → 授予 ActivateOnGiven + Reactive
    ② 遍历 HeroStartUpAbilitySets:
         给每个技能创建 Spec
         把 InputTag 贴到 DynamicSpecSourceTags 上
         → 这样按键时 ASC 能找到对应的技能

关键变量：
  HeroStartUpAbilitySets[] — FWarriorHeroAbilitySet 数组
    每个元素: { InputTag, TSubclassOf<UWarriorGameplayAbility> }
```

---

### 输入

#### `UWarriorInputComponent`（[WarriorInputComponent.h](Source/Warrior/Public/Components/Input/WarriorInputComponent.h)）

```
作用：封装 Enhanced Input 的绑定逻辑

做了什么：
  BindNativeInputAction(Config, Tag, TriggerEvent, Context, Func):
    从 Config 中找到 Tag 对应的 InputAction
    BindAction(Action, TriggerEvent, Context, Func)
    → 用于 Move/Look（直接调 C++ 函数）

  BindAbilityInputAction(Config, Context, PressedFunc, ReleasedFunc):
    遍历 Config 的所有 AbilityInputActions
    每个都绑定 PressedFunc（带 Tag 参数）和 ReleasedFunc
    → 用于技能输入（走 GAS）
```

---

### 辅助类

#### `UPawnExtensionComponentBase`（[PawnExtensionComponentBase.h](Source/Warrior/Public/Components/PawnExtensionComponentBase.h)）

```
作用：组件基类，提供两个模板函数
内容：GetOwningPawn<T>() 和 GetOwningController<T>()
用途：目前没有被调用过，留作将来扩展
```

#### `FWarriorHeroWeaponTypes`（[WarriorStructTypes.h](Source/Warrior/Public/WarriorTypes/WarriorStructTypes.h)）

```
作用：英雄武器的配置数据结构

字段：
  WeaponAnimLayerToLink    — TSubclassOf<UWarriorHeroLinkedAnimLayer>
                             装备时链接的动画层（例如 AnimLayer_HeroAxe）
  
  WeaponInputMappingContext — TObjectPtr<UInputMappingContext>
                              装备时激活的输入映射（例如 IMC_Axe）
```

#### `FWarriorHeroAbilitySet`（[WarriorStructTypes.h](Source/Warrior/Public/WarriorTypes/WarriorStructTypes.h)）

```
作用：一对"按键Tag → 技能"的映射

字段：
  InputTag        — FGameplayTag（例如 InputTag.EquipAxe）
  AbilityToGrant  — TSubclassOf<UWarriorGameplayAbility>（例如 GA_Hero_EquipAxe）
  
函数：
  IsValid() → InputTag 有效 且 AbilityToGrant 不为空
```

---

## 九、蓝图逐个解释

### 动画蓝图

| 蓝图 | 路径 | 父类 | 作用 |
|------|------|------|------|
| **ABP_Hero** | `Content/BP/AnimBP/ABP_Hero.uasset` | `UWarriorHeroAnimInstance` | 角色主动画蓝图。读取 `GroundSpeed`、`bShouldEnterRelaxState` 来驱动动画状态机。有一个 `ArmedLocomotionState` 插槽可以被链接层覆盖。 |
| **ALI_Hero** | `Content/BP/AnimBP/ALI_Hero.uasset` | `AnimLayerInterface` | 动画层接口。只声明了一个函数 `ArmedLocomotionState`——任何实现这个接口的链接层都必须提供这个动画状态。 |
| **MasterAnimLayer_Hero** | `Content/BP/AnimBP/MasterAnimLayer_Hero.uasset` | `UWarriorHeroLinkedAnimLayer` | 默认链接层。实现 `ALI_Hero`。每帧调用 `GetHeroAnimInstance()` 读 `GroundSpeed`，喂给 BlendSpace。空手状态下提供所有移动动画。 |
| **AnimLayer_HeroAxe** | `Content/BP/AnimBP/AnimLayer/AnimLayer_HeroAxe.uasset` | `MasterAnimLayer_Hero`（蓝图继承） | 斧头链接层。覆盖 `DefaultLocomotionBlendSpace` 为 `BS_DefaultLocomotion_Axe`。持斧状态下提供所有移动动画。 |
| **BS_DefaultLocomotion_Axe** | `Content/BP/AnimBP/BS_DefaultLocomotion_Axe.uasset` | BlendSpace1D | 斧头移动混合空间。输入横轴 `GroundSpeed`，输出混合后的持斧移动姿势。 |
| **BS_UnarmedLocomotion** | `Content/BP/AnimBP/BlendSpace/BS_UnarmedLocomotion.uasset` | BlendSpace | 空手移动混合空间。 |

### 蒙太奇

| 蓝图 | 路径 | 内容 |
|------|------|------|
| **Hero_Axe_Equip_Montage** | `Content/BP/Montage/Hero_Axe_Equip_Montage.uasset` | 装备斧头动画。引用 `Hero_Axe_Equip` 动画序列。包含 `AN_SendGameplayEventToOwner` 通知，在关键时刻发送 `Player.Event.Equip.Axe`。 |
| **Hero_Axe_Unequip_Montage** | `Content/BP/Montage/Hero_Axe_Unequip_Montage.uasset` | 卸载斧头动画。发送 `Player.Event.Unequip.Axe`。 |

### 技能蓝图

| 蓝图 | 路径 | 父类 | 激活方式 | 作用 |
|------|------|------|---------|------|
| **GA_Shared_SpawnWeapon** | `Content/Shared/GameplayAbility/` | `UWarriorGameplayAbility` | OnGiven | 通用武器生成技能。生成一个武器 Actor 并注册到 CombatComponent。 |
| **GA_Hero_Spawn_Axe** | `Content/BP/GameplayAbility/` | 继承 GA_Shared_SpawnWeapon | OnGiven | 生成斧头。游戏开始时自动激活。 |
| **GA_Hero_EquipAxe** | `Content/BP/GameplayAbility/` | `UWarriorHeroGameplayAbility` | OnTriggered | 装备斧头。按"1"激活。播放蒙太奇 + 等待 GameplayEvent + 挂接武器 + 链接动画层 + 激活 IMC_Axe。 |
| **GA_Hero_UnequipAxe** | `Content/BP/GameplayAbility/` | `UWarriorHeroGameplayAbility` | OnTriggered | 卸载斧头。和装备对称。 |

### 武器蓝图

| 蓝图 | 路径 | 父类 | 作用 |
|------|------|------|------|
| **BP_HeroWeaponBase** | `Content/BP/HeroWeapon/` | `AWarriorHeroWeapon` | 英雄武器的基类蓝图。设置碰撞盒大小等默认值。 |
| **BP_HeroAxe** | `Content/BP/HeroWeapon/` | `BP_HeroWeaponBase` | 斧头。设置 3D 模型 `SM_DefaultAxe`。`HeroWeaponData` 里配了 `AnimLayer_HeroAxe` 和 `IMC_Axe`。 |

### 输入蓝图

| 蓝图 | 路径 | 内容 |
|------|------|------|
| **IMC_Default** | `Content/BP/Input/IMC_Default.uasset` | 默认按键映射：键位 `1` → `IA_EquipAxe`，W/A/S/D → `IA_Move`，鼠标 → `IA_Look` |
| **IMC_Axe** | `Content/BP/Input/IMC_Axe.uasset` | 持斧状态按键映射：攻击键等（装备斧头时激活） |
| **IA_EquipAxe** | `Content/BP/Input/Actions/IA_EquipAxe.uasset` | 装备斧头输入动作 |
| **IA_UnEquipAxe** | `Content/BP/Input/Actions/IA_UnEquipAxe.uasset` | 卸载斧头输入动作 |
| **IA_Move** | `Content/BP/Input/Actions/IA_Move.uasset` | 移动输入动作 |
| **IA_Look** | `Content/BP/Input/Actions/IA_Look.uasset` | 视角输入动作 |

### 数据资产蓝图

| 蓝图 | 路径 | 父类 | 内容 |
|------|------|------|------|
| **DA_Hero** | `Content/BP/DA_Hero.uasset` | `UDataAsset_HeroStartUpData` | `ActivateOnGivenAbilities` 里有 `GA_Hero_Spawn_Axe`；`HeroStartUpAbilitySets` 里有 `InputTag.EquipAxe → GA_Hero_EquipAxe` |
| **DA_InputConfig** | `Content/BP/DA_InputConfig.uasset` | `UDataAsset_InputConfig` | `NativeInputActions` 有 Move/Look；`AbilityInputActions` 有 EquipAxe/UnequipAxe |

### AnimNotify

| 蓝图 | 路径 | 作用 |
|------|------|------|
| **AN_SendGameplayEventToOwner** | `Content/Shared/AnimNotify/AN_SendGameplayEventToOwner.uasset` | 蒙太奇中使用的通知。在动画播放到特定帧时，向角色发送 GameplayEvent。装备蒙太奇发送 `Player.Event.Equip.Axe`，卸载蒙太奇发送 `Player.Event.Unequip.Axe`。 |

---

## 十、数据怎么流动（速查表）

### 角色移动 → 动画

```
按键 W/A/S/D
  → AddMovementInput()
  → CharacterMovement 更新速度
  → NativeThreadSafeUpdateAnimation 读速度 → GroundSpeed
  → 动画蓝图读 GroundSpeed → BlendSpace → 最终动画
```

### 按 "1" → 装备斧头

```
按键 "1"
  → IA_EquipAxe → InputTag.EquipAxe
  → ASC::OnAbilityInputPressed → TryActivateAbility(GA_Hero_EquipAxe)
  → 播放 Hero_Axe_Equip_Montage
  → AnimNotify 发送 Player.Event.Equip.Axe
  → WaitGameplayEvent 回调
  → 从 CombatComponent 取斧头 → 挂到手 → LinkAnimClassLayers → 动画切换
```

### 武器数据流

```
BP_HeroAxe.HeroWeaponData
  ├─ WeaponAnimLayerToLink = AnimLayer_HeroAxe
  │     └─ GA_Hero_EquipAxe 回调用 LinkAnimClassLayers(this)
  │         └─ 覆盖 ABP_Hero 的 ArmedLocomotionState → 持斧动画
  │
  └─ WeaponInputMappingContext = IMC_Axe
        └─ GA_Hero_EquipAxe 回调用 AddMappingContext(this)
            └─ 激活持斧按键
```

### 类查询级联

```
需要从技能里拿东西时:

  UWarriorHeroGameplayAbility:
    GetHeroCombatComponentFromActorInfo()
      → GetHeroCharacterFromActorInfo()  [缓存]
         → GetHeroCombatComponent()
            → UHeroCombatComponent::GetHeroCharacterCarriedWeaponByTag(Tag)
               → UPawnCombatComponet::GetCharacterCarriedWeaponByTag(Tag)
                  → CharacterCarriedWeaponMap.Find(Tag)

需要从动画层拿东西时:

  UWarriorHeroLinkedAnimLayer:
    GetHeroAnimInstance()
      → GetOwningComponent()  [SkeletalMeshComponent]
         → GetAnimInstance()  [UWarriorHeroAnimInstance / ABP_Hero]
            → 读 GroundSpeed, bShouldEnterRelaxState
```

---

## 附录：关键文件路径速查

```
核心角色:
  Source/Warrior/Public/Characters/WarriorBaseCharacter.h
  Source/Warrior/Public/Characters/WarriorHeroCharacter.h

核心技能:
  Source/Warrior/Public/AbilitySystem/WarriorAbilitySystemComponent.h
  Source/Warrior/Public/AbilitySystem/Abilities/WarriorGameplayAbility.h
  Source/Warrior/Public/AbilitySystem/Abilities/WarriorHeroGameplayAbility.h

核心动画:
  Source/Warrior/Public/AnimInstances/WarriorCharacterAnimInstance.h
  Source/Warrior/Public/AnimInstances/Hero/WarriorHeroAnimInstance.h
  Source/Warrior/Public/AnimInstances/Hero/WarriorHeroLinkedAnimLayer.h

核心战斗:
  Source/Warrior/Public/Components/Combat/PawnCombatComponet.h
  Source/Warrior/Public/Components/Combat/HeroCombatComponent.h
  Source/Warrior/Public/Items/Weapons/WarriorWeaponBase.h
  Source/Warrior/Public/Items/Weapons/WarriorHeroWeapon.h

核心数据:
  Source/Warrior/Public/DataAssets/StartUpData/DataAsset_StartUpDataBase.h
  Source/Warrior/Public/DataAssets/StartUpData/DataAsset_HeroStartUpData.h
  Source/Warrior/Public/DataAssets/Input/DataAsset_InputConfig.h
  Source/Warrior/Public/WarriorTypes/WarriorStructTypes.h
  Source/Warrior/Public/WarriorGameplayTags.h

核心输入:
  Source/Warrior/Public/Components/Input/WarriorInputComponent.h
