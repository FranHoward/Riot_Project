# Week 01：Unreal Engine 5 基础学习周报

## 本周目标

本周主要完成 Unreal Engine 5 开发环境搭建，熟悉编辑器的基本界面与工作流程，并通过概念对照和 Blueprint 实践，初步建立从其他引擎迁移到 UE 的知识框架。

## 本周完成内容

### 1. 搭建 UE5 C++ 开发环境

- 通过 Epic Games Launcher 安装 Unreal Engine 5（5.3 或以上版本）。
- 在 Windows 环境中安装 Visual Studio Community，并勾选“使用 C++ 的游戏开发”工作负载及 UE 开发所需组件。
- 确认 UE 能够识别编译工具链，为后续使用 C++ 编写游戏功能做好准备。

### 2. 创建并熟悉第三人称 C++ 工程

- 按照 **Games → Third Person → C++** 的路径创建第三人称模板工程，没有使用纯 Blueprint 模板。
- 打开模板示例地图，查看 Character、Floor、Light 等不同类型的 Actor。
- 熟悉以下常用编辑器面板：
  - **Content Browser**：管理地图、Blueprint、材质、模型等项目资源。
  - **World Outliner**：查看和管理当前关卡中的 Actor。
  - **Details**：查看 Actor 的属性、Transform 和 Component 树，并修改可编辑参数。
  - **Output Log**：查看运行信息、调试输出和错误提示。

### 3. 整理引擎概念迁移对照表

完成了 [Unity 与 Unreal Engine 概念对照表](../Comparison.md)，主要覆盖以下 8 组概念：

| Unity / 通用概念 | Unreal Engine 对应概念 |
| --- | --- |
| 场景物体 | Actor |
| 可复用预制体 | Blueprint Class |
| 行为脚本 | Actor 自身事件或 ActorComponent |
| 初始化时机 | Construction Script / BeginPlay |
| 每帧更新 | Tick |
| 实例化 / 销毁 | SpawnActor / DestroyActor |
| 面板可编辑字段 | `UPROPERTY(EditAnywhere)` |
| 预制体变体 | Blueprint 子类 |

通过整理这些概念，我理解了 UE 主要围绕 Actor、Component、Blueprint Class 和类继承组织游戏对象及其行为，也认识到不同引擎中的对应概念功能相近，但不能简单地按名称一一替换。

### 4. 使用 Blueprint 制作可交互收集物

- 新建一个继承自 Actor 的 Blueprint Class。
- 添加 `StaticMeshComponent` 作为收集物的可视模型。
- 添加 `SphereCollision` 作为触发区域，并配置重叠检测。
- 为收集物添加持续旋转效果，使其在关卡中更容易被玩家识别。
- 在 Event Graph 中使用 `Event ActorBeginOverlap` 监听玩家进入触发区域的事件。
- 重叠发生后先调用 `Print String` 显示提示信息，验证事件已经成功触发，再调用 `DestroyActor` 移除收集物。
- 在编辑器中运行游戏并测试靠近、触发提示和销毁的完整流程，同时录制运行效果。

## 实现结果

最终完成了一个能够在关卡中旋转、检测玩家靠近、输出交互提示并在触发后销毁的 Blueprint 收集物。测试结果表明碰撞重叠事件和销毁逻辑均能正常执行。

## 本周总结

通过本周练习，我完成了 UE5 C++ 开发环境和第三人称工程的基础搭建，掌握了编辑器核心面板的基本用途，并理解了 Actor、Component、Blueprint 和生命周期事件等关键概念。可交互收集物的制作也让我熟悉了 Blueprint 的组件组合、事件驱动逻辑和基础调试方法，为下一阶段使用 C++ 实现游戏功能打下了基础。
