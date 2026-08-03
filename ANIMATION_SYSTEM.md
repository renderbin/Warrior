# Warrior 项目动画系统架构文档

> 本文档供 AI 辅助工具理解项目动画系统，涵盖 C++ 类层次、蓝图资产关系、数据流、装备/卸载流程。

---

## 1. C++ 类继承层次

```
UAnimInstance (引擎)
 └─ UWarriorBaseAnimInstance                    [空基类，无额外逻辑]
      ├─ UWarriorCharacterAnimInstance          [每帧计算: GroundSpeed, bHasAcceleration]
      │    └─ UWarriorHeroAnimInstance           [每帧计算: bShouldEnterRelaxState, IdleElapsedTime]
      │         └─ ABP_Hero (蓝图)               [主动画蓝图，角色 SkeletalMeshComponent 使用]
      │
      └─ UWarriorHeroLinkedAnimLayer            [链接动画层基类，提供 GetHeroAnimInstance()]
           └─ MasterAnimLayer_Hero (蓝图)        [默认链接层: 空手移动动画，读取主动画实例变量]
                └─ AnimLayer_HeroAxe (蓝图)       [斧头链接层: 覆盖移动动画为持斧版本]
```

### 关键点

- **主动画实例 (ABP_Hero)** 和 **链接动画层 (MasterAnimLayer_Hero)** 是两个**独立的 UAnimInstance**，挂载在同一个 SkeletalMeshComponent 上
- 链接动画层通过 `GetOwningComponent()->GetAnimInstance()` 获取主动画实例的引用
- `AnimLayer_HeroAxe` 的**父类是 MasterAnimLayer_Hero 蓝图**（而非直接继承 C++），这是蓝图继承

---

## 2. 动画层三层架构

### 第一层: ALI_Hero (动画层接口)

```
类型: Animation Layer Interface (引擎内置 AnimLayerInterface)
父类: AnimLayerInterface
路径: Content/BP/AnimBP/ALI_Hero.uasset
BlueprintType: BPTYPE_Interface

声明了一个函数:
  ArmedLocomotionState → PoseLink
```

这是"合同"——任何链接动画层都必须提供 `ArmedLocomotionState` 动画状态。
ABP_Hero 在其 AnimGraph 中调用 `ArmedLocomotionState` 来获取武器特定的运动动画。

### 第二层: MasterAnimLayer_Hero (默认链接层)

```
类型: Linked Animation Blueprint
C++ 父类: UWarriorHeroLinkedAnimLayer
实现的接口: ALI_Hero
路径: Content/BP/AnimBP/MasterAnimLayer_Hero.uasset
用途: 空手状态下的默认移动动画

事件图逻辑:
  BlueprintUpdateAnimation
    → GetHeroAnimInstance()            [C++ 函数，WarriorHeroLinkedAnimLayer 提供]
    → 读取 GroundSpeed                  [从主动画实例读取]
    → 传递给 BlendSpacePlayer

动画图:
  ArmedLocomotionState (实现 ALI_Hero 接口)
    └─ BlendSpacePlayer(DefaultLocomotionBlendSpace)
         输入: GroundSpeed (0~400+)
         输出: 空手 Idle/Walk/Jog/Turn 动画混合
```

### 第三层: AnimLayer_HeroAxe (武器特定链接层)

```
类型: Linked Animation Blueprint
蓝图父类: MasterAnimLayer_Hero        [注意: 蓝图继承，不是 C++ 继承]
C++ 最终父类: UWarriorHeroLinkedAnimLayer
路径: Content/BP/AnimBP/AnimLayer/AnimLayer_HeroAxe.uasset
用途: 持斧状态下的移动动画

关键覆盖:
  DefaultLocomotionBlendSpace → BS_DefaultLocomotion_Axe
  BS_DefaultLocomotion_Axe 包含:
    Hero_Axe_Walk_Fwd / Walk_Left / Walk_Right / Walk_Bwd / Walk_FwdLeft / Walk_FwdRight
    Hero_Axe_JogFwd_Loop
    Hero_Axe_TurnLeft_90 / TurnRight_90
    Hero_AxeBlock / Hero_AxeBlockReactLarge
```

---

## 3. 完整数据流: 玩家移动到动画播放

```
① 玩家按下移动键 (W/A/S/D)
    ↓
② Enhanced Input → AWarriorHeroCharacter::Input_Move()
    ↓
③ AddMovementInput() → UCharacterMovementComponent
    ↓
④ UWarriorCharacterAnimInstance::NativeThreadSafeUpdateAnimation() [C++ 后台线程安全]
    GroundSpeed = GetVelocity().Size2D()
    bHasAcceleration = GetCurrentAcceleration().SizeSquared2D() > 0
    ↓
⑤ UWarriorHeroAnimInstance::NativeUpdateAnimation() [C++ 游戏线程]
    如果 bHasAcceleration: bShouldEnterRelaxState = false, IdleElapsedTime = 0
    否则: IdleElapsedTime += DeltaTime
          如果 IdleElapsedTime >= 5s: bShouldEnterRelaxState = true
    ↓
⑥ MasterAnimLayer_Hero 或 AnimLayer_HeroAxe 的 BlueprintUpdateAnimation [蓝图事件]
    GetHeroAnimInstance() → 读取 GroundSpeed, bShouldEnterRelaxState
    ↓
⑦ BlendSpacePlayer 根据 GroundSpeed 选择动画
    空手:   0→Idle  100→Walk  300→Jog
    持斧:   0→Idle_Axe  100→Walk_Axe  300→Jog_Axe
    ↓
⑧ PoseLink → ABP_Hero 的 ArmedLocomotionState 槽位 → 最终输出到屏幕
```

---

## 4. 武器系统与动画的交互

### 武器生成 (游戏启动时自动)

```
GA_Hero_Spawn_Axe (ActivationPolicy = OnGiven)
  → SpawnActor(BP_HeroAxe)
  → PawnCombatComponent::RegisterSpawnedWeapon("Player.Weapon.Axe", Axe, bEquipped=false)
```

**BP_HeroAxe 的关键属性:**
```
HeroWeaponData (FWarriorHeroWeaponTypes):
  ├─ WeaponAnimLayerToLink = AnimLayer_HeroAxe    [装备时链接此动画层]
  └─ WeaponInputMappingContext = IMC_Axe           [装备时激活此输入映射]
```

### 武器装备 (按"1"键时)

```
按键 "1"
  → IA_EquipAxe → InputTag.EquipAxe
  → WarriorAbilitySystemComponent::OnAbilityInputPressed("InputTag.EquipAxe")
  → 查找 DynamicSpecSourceTags 中具有该 Tag 的能力
  → TryActivateAbility(GA_Hero_EquipAxe)

GA_Hero_EquipAxe 激活:
  ① UAbilityTask_PlayMontageAndWait(Hero_Axe_Equip_Montage)
     蒙太奇播放装备动画，包含 AN_SendGameplayEventToOwner AnimNotify
     该通知发送 GameplayEvent("Player.Event.Equip.Axe")

  ② UAbilityTask_WaitGameplayEvent("Player.Event.Equip.Axe")
     等待 AnimNotify 触发

  ③ 当事件被触发时 (OnEventReceived):
      a. GetHeroCarriedWeaponByTag("Player.Weapon.Axe")
         → 从 PawnCombatComponent 的 TMap 中检索斧头
      b. Break HeroWeaponData → 获取 WeaponAnimLayerToLink, WeaponInputMappingContext
      c. K2_AttachToComponent(斧头, Mesh, "AxeRightHandSocket")
         → 物理上将斧头挂载到角色右手骨骼
      d. LinkAnimClassLayers(AnimLayer_HeroAxe)
         → 将 AnimLayer_HeroAxe 注入 SkeletalMeshComponent
         → ABP_Hero 的 ArmedLocomotionState 现在输出持斧动画
      e. CurrentEquippedWeaponTag = "Player.Weapon.Axe"
         → 标记当前装备的武器
      f. AddMappingContext(IMC_Axe)
         → 激活持斧输入映射（攻击键等）
```

---

## 5. 关键 C++ 类职责速查

| 类 | 文件 | 职责 |
|----|------|------|
| `UWarriorCharacterAnimInstance` | `AnimInstances/WarriorCharacterAnimInstance.h` | 每帧从 MovementComponent 计算 GroundSpeed、bHasAcceleration |
| `UWarriorHeroAnimInstance` | `AnimInstances/Hero/WarriorHeroAnimInstance.h` | 空闲计时器 → bShouldEnterRelaxState，缓存 OwningHeroCharacter |
| `UWarriorHeroLinkedAnimLayer` | `AnimInstances/Hero/WarriorHeroLinkedAnimLayer.h` | `GetHeroAnimInstance()`: 从主动画实例读取变量 |
| `UWarriorBaseAnimInstance` | `AnimInstances/WarriorBaseAnimInstance.h` | 空基类，所有 Warrior AnimInstance 的根 |
| `UWarriorAbilitySystemComponent` | `AbilitySystem/WarriorAbilitySystemComponent.h` | `OnAbilityInputPressed(tag)`: 输入→能力激活路由 |
| `UPawnCombatComponent` | `Components/Combat/PawnCombatComponent.h` | TMap<FGameplayTag, AWarriorWeaponBase*> 武器注册，CurrentEquippedWeaponTag |
| `UHeroCombatComponent` | `Components/Combat/HeroCombatComponent.h` | `GetHeroCarriedWeaponByTag()`: 按 Tag 查找并 Cast 武器 |

---

## 6. 关键蓝图资产速查

| 资产 | 路径 | 类型 | 用途 |
|------|------|------|------|
| ABP_Hero | `Content/BP/AnimBP/ABP_Hero.uasset` | AnimBlueprint | 角色主动画蓝图，父类 UWarriorHeroAnimInstance |
| ALI_Hero | `Content/BP/AnimBP/ALI_Hero.uasset` | AnimLayerInterface | 声明 ArmedLocomotionState 函数槽位 |
| MasterAnimLayer_Hero | `Content/BP/AnimBP/MasterAnimLayer_Hero.uasset` | LinkedAnimBP | 默认空手移动层，实现 ALI_Hero |
| AnimLayer_HeroAxe | `Content/BP/AnimBP/AnimLayer/AnimLayer_HeroAxe.uasset` | LinkedAnimBP | 斧头移动层，继承 MasterAnimLayer_Hero |
| BS_DefaultLocomotion_Axe | `Content/BP/AnimBP/BS_DefaultLocomotion_Axe.uasset` | BlendSpace1D | 持斧移动混合空间 |
| BS_UnarmedLocomotion | `Content/BP/AnimBP/BlendSpace/BS_UnarmedLocomotion.uasset` | BlendSpace | 空手移动混合空间 |
| Hero_Axe_Equip_Montage | `Content/BP/Montage/Hero_Axe_Equip_Montage.uasset` | AnimMontage | 装备斧头蒙太奇，包含 AN_SendGameplayEventToOwner |
| AN_SendGameplayEventToOwner | `Content/Shared/AnimNotify/AN_SendGameplayEventToOwner.uasset` | AnimNotify | 发送 GameplayEvent 给角色所有者 |
| BP_HeroAxe | `Content/BP/HeroWeapon/BP_HeroAxe.uasset` | Blueprint | 斧头 Actor，HeroWeaponData 指向 AnimLayer_HeroAxe + IMC_Axe |
| GA_Hero_EquipAxe | `Content/BP/GameplayAbility/GA_Hero_EquipAxe.uasset` | GameplayAbility | 装备能力：播放蒙太奇+等待事件+链接动画层 |
| GA_Hero_Spawn_Axe | `Content/BP/GameplayAbility/GA_Hero_Spawn_Axe.uasset` | GameplayAbility | 生成斧头能力：OnGiven 自动激活 |
| DA_Hero | `Content/BP/DA_Hero.uasset` | DataAsset | 英雄启动数据：能力列表+InputTag 绑定 |
| DA_InputConfig | `Content/BP/DA_InputConfig.uasset` | DataAsset | 输入配置：InputAction→InputTag 映射 |
| IMC_Default | `Content/BP/Input/IMC_Default.uasset` | InputMappingContext | 默认按键映射：键位"1"→IA_EquipAxe |

---

## 7. GameplayTag 完整列表

| Tag | 用途 |
|-----|------|
| `InputTag.Move` | 移动输入 |
| `InputTag.Look` | 视角输入 |
| `InputTag.EquipAxe` | 装备斧头输入（绑定到键位"1"） |
| `InputTag.UnequipAxe` | 卸载斧头输入 |
| `Player.Weapon.Axe` | 角色携带的斧头武器的注册键 |
| `Player.Event.Equip.Axe` | AnimNotify 触发 → 实际执行装备逻辑的事件 |
| `Player.Event.Unequip.Axe` | AnimNotify 触发 → 实际执行卸载逻辑的事件 |

---

## 8. 动画资产目录结构

```
Content/Assets/HeroCharacter/Animations/Axe/
  ├─ Hero_Axe_Equip.uasset              [装备动画序列]
  ├─ Hero_Axe_Unequip.uasset            [卸载动画序列]
  ├─ Hero_Axe_Walk_Fwd.uasset           [持斧前进行走]
  ├─ Hero_Axe_Walk_Left.uasset          [持斧左行走]
  ├─ Hero_Axe_Walk_Right.uasset         [持斧右行走]
  ├─ Hero_Axe_Walk_Bwd.uasset           [持斧后退行走]
  ├─ Hero_Axe_Walk_FwdLeft.uasset       [持斧前左行走]
  ├─ Hero_Axe_Walk_FwdRight.uasset      [持斧前右行走]
  ├─ Hero_Axe_JogFwd_Loop.uasset        [持斧前进慢跑]
  ├─ Hero_Axe_TurnLeft_90.uasset        [持斧左转]
  ├─ Hero_Axe_TurnRight_90.uasset       [持斧右转]
  ├─ Hero_Axe_LightAttack_1~4.uasset    [轻重攻击动画]
  ├─ Hero_Axe_HeavyAttack_1~2.uasset    [重攻击动画]
  ├─ Hero_AxeBlock.uasset               [格挡动画]
  ├─ Hero_Axe_Shout.uasset              [怒吼动画]
  ├─ Hero_Idle_Axe.uasset               [持斧空闲]
  ├─ Hero_Idle_AxeRelax_1~2.uasset      [持斧放松空闲]
  └─ Hero_Death_Fallback.uasset         [死亡动画]

Content/Assets/HeroCharacter/Animations/ (无武器)
  ├─ Hero_Unarmed_Idle.uasset
  ├─ Hero_Unarmed_Idle_Relax_1~2.uasset
  ├─ Hero_Unarmed_Jog_Fwd.uasset
  ├─ Hero_Unarmed_TurnLeft90.uasset
  ├─ Hero_Unarmed_TurnRight90.uasset
  ├─ Hero_HitReact_*.uasset
  ├─ Hero_Roll.uasset
  └─ Hero_Sword_Death_1.uasset
```

---

## 9. 设计决策与注意事项

1. **C++ 只做数据计算，蓝图只做数据绑定。** `NativeThreadSafeUpdateAnimation` 计算 `GroundSpeed` 和 `bHasAcceleration`，托管蓝图读取这些变量并驱动混合空间。这使得混合空间的调整可以即时进行，无需 C++ 重新编译。

2. **链接动画层继承自蓝图，而非 C++。** `AnimLayer_HeroAxe` 的父类是 `MasterAnimLayer_Hero`（蓝图），而不是 `UWarriorHeroLinkedAnimLayer`（C++）。这允许在不触及 C++ 代码的情况下覆盖 `DefaultLocomotionBlendSpace` 变量。

3. **`GetHeroAnimInstance()` 是链接层和主动画实例之间的桥梁。** 它通过 `GetOwningComponent()->GetAnimInstance()` 访问——两者都在同一个 SkeletalMeshComponent 上，因此链接层可以读取主动画实例暴露的所有蓝图变量。

4. **`bShouldEnterRelaxState` 是一个 Hero 特有的概念。** 基类 `UWarriorCharacterAnimInstance` 不知道放松状态——只有 `UWarriorHeroAnimInstance` 知道。这保持了角色动画类的清洁可组合。

5. **武器生成 和 武器装备是分开的。** 生成在启动时通过 `OnGiven` 能力进行，装备在按键时通过 `OnTriggered` 能力进行。中间状态——"武器存在但未装备"——是使装备蒙太奇看起来自然的设计。
