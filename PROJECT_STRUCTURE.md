# Warrior 项目结构文档

> 记录类继承层次、文件位置、后续课程新建规划。

## 1. 类继承结构

### 🎮 游戏模式

- **WarriorBaseGameMode**（顶层游戏模式）`GameModes/WarriorBaseGameMode`

### 🧍 角色

```
- **WarriorBaseCharacter**（基础角色类）
  - **WarriorHeroCharacter**（英雄角色，玩家控制）
  - WarriorEnemyCharacter（敌方角色，后续课程）
```

### 🎯 控制器

```
- **WarriorHeroController**（玩家控制器，区分 AI 与玩家）
- WarriorEnemyController（敌方 AI 控制器，后续课程）
```

### ⚙️ 组件

```
- **PawnExtensionComponentBase**（辅助函数，无业务逻辑）
  - **PawnCombatComponent**（敌我共用战斗逻辑）
    - **HeroCombatComponent**（玩家战斗逻辑）
    - EnemyCombatComponent（敌人战斗逻辑，后续课程）
```

### 🏃 动画实例

```
- **WarriorBaseAnimInstance**（基类，提供辅助函数）
  - **WarriorCharacterAnimInstance**（共享逻辑，玩家和敌人共用）
    - WarriorHeroAnimInstance（玩家特定代码，后续课程）
    - WarriorEnemyAnimInstance（敌方动画，后续课程）
  - WarriorHeroLinkedAnimLayer（链接动画层，用于武器动画，后续课程）
```

### ✨ 能力系统

```
- **WarriorGameplayAbility**（基础技能基类）
  - **WarriorHeroGameplayAbility**（英雄技能）
  - WarriorEnemyGameplayAbility（敌方技能，后续课程）
- **WarriorAbilitySystemComponent**（ASC，能力系统组件）
- **WarriorAttributeSet**（属性集）
```

### 📦 数据资产

```
- **DataAsset_StartUpDataBase**（启动数据基类）
  - **DataAsset_HeroStartUpData**（英雄启动数据）
  - DataAsset_EnemyStartUpData（敌方启动数据，后续课程）
- **DataAsset_InputConfig**（输入配置数据）
```

### ⚔️ 武器

```
- **WarriorWeaponBase**（武器基类）
  - **WarriorHeroWeapon**（英雄武器）
  - WarriorEnemyWeapon（敌方武器，后续课程）
```

### 🧰 其他

```
- **WarriorGameplayTags**（游戏标签定义）
- **WarriorFunctionLibrary**（静态函数库）
```

## 2. 文件清单

### 游戏模式

| 文件 | 类型 | 路径 |
| ---- | ---- | ---- |
| WarriorBaseGameMode.h/.cpp | C++ | `Source/Warrior/GameModes/` |

### 角色

| 文件 | 类型 | 路径 |
| ---- | ---- | ---- |
| WarriorBaseCharacter.h/.cpp | C++ | `Source/Warrior/Characters/` |
| WarriorHeroCharacter.h/.cpp | C++ | `Source/Warrior/Characters/` |

### 控制器

| 文件 | 类型 | 路径 |
| ---- | ---- | ---- |
| WarriorHeroController.h/.cpp | C++ | `Source/Warrior/Controllers/` |

### 组件

| 文件 | 类型 | 路径 |
| ---- | ---- | ---- |
| PawnExtensionComponentBase.h/.cpp | C++ | `Source/Warrior/Components/` |
| PawnCombatComponent.h/.cpp | C++ | `Source/Warrior/Components/Combat/` |
| HeroCombatComponent.h/.cpp | C++ | `Source/Warrior/Components/Combat/` |

### 动画实例

| 文件 | 类型 | 路径 |
| ---- | ---- | ---- |
| WarriorBaseAnimInstance.h/.cpp | C++ | `Source/Warrior/AnimInstances/` |
| WarriorCharacterAnimInstance.h/.cpp | C++ | `Source/Warrior/AnimInstances/` |
| WarriorHeroAnimInstance.h/.cpp | C++ | `Source/Warrior/AnimInstances/Hero/` |
| WarriorHeroLinkedAnimLayer.h/.cpp | C++ | `Source/Warrior/AnimInstances/` |

### 能力系统

| 文件 | 类型 | 路径 |
| ---- | ---- | ---- |
| WarriorAbilitySystemComponent.h/.cpp | C++ | `Source/Warrior/AbilitySystem/` |
| WarriorAttributeSet.h/.cpp | C++ | `Source/Warrior/AbilitySystem/` |
| WarriorGameplayAbility.h/.cpp | C++ | `Source/Warrior/AbilitySystem/Abilities/` |
| WarriorHeroGameplayAbility.h/.cpp | C++ | `Source/Warrior/AbilitySystem/Abilities/` |

### 数据资产

| 文件 | 类型 | 路径 |
| ---- | ---- | ---- |
| DataAsset_InputConfig.h/.cpp | C++ | `Source/Warrior/DataAssets/Input/` |
| DataAsset_StartUpDataBase.h/.cpp | C++ | `Source/Warrior/DataAssets/StartUpData/` |
| DataAsset_HeroStartUpData.h/.cpp | C++ | `Source/Warrior/DataAssets/StartUpData/` |

### 武器

| 文件 | 类型 | 路径 |
| ---- | ---- | ---- |
| WarriorWeaponBase.h/.cpp | C++ | `Source/Warrior/Items/Weapons/` |
| WarriorHeroWeapon.h/.cpp | C++ | `Source/Warrior/Items/Weapons/` |

### 其他

| 文件 | 类型 | 路径 |
| ---- | ---- | ---- |
| WarriorGameplayTags.h/.cpp | C++ | `Source/Warrior/` |
| WarriorFunctionLibrary.h/.cpp | C++ | `Source/Warrior/` |

## 3. 后续课程新建规划

| 文件/文件夹 | 位置 | 说明 |
| ----------- | ---- | ---- |
| WarriorEnemyCharacter.h/.cpp | `Characters/` | 敌方角色 |
| WarriorEnemyController.h/.cpp | `Controllers/` | 敌方 AI 控制器 |
| EnemyCombatComponent.h/.cpp | `Components/Combat/` | 敌人战斗逻辑 |
| WarriorEnemyAnimInstance.h/.cpp | `AnimInstances/Enemy/` | 敌方动画（需新建 `Enemy/` 文件夹） |
| WarriorEnemyGameplayAbility.h/.cpp | `AbilitySystem/Abilities/` | 敌方技能 |
| DataAsset_EnemyStartUpData.h/.cpp | `DataAssets/StartUpData/` | 敌方启动数据 |
| WarriorEnemyWeapon.h/.cpp | `Items/Weapons/` | 敌方武器 |
| WarriorStructTypes.h | `Source/Warrior/Public/WarriorTypes/` | 共享结构体定义（需新建 `WarriorTypes/` 文件夹） |

> 唯一需要新建的**文件夹**：`AnimInstances/Enemy/`、`Public/WarriorTypes/`；其余敌方类直接放入已有文件夹，与 Hero 类一一对称。
