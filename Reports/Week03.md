# Week 03：第三人称角色与 Enhanced Input 周报

## 本周目标

本周主要实现一个可操控的第三人称角色，掌握 Enhanced Input 系统中 Input Action 与 Input Mapping Context 的资源化配置方式，并完成角色移动、跳跃、转向及基础跟随摄像机功能。

## 本周完成内容

### 1. 分析第三人称角色模板与 Character 组件

- 阅读 Third Person 模板中的角色 Blueprint 与 C++ 代码，了解 `Character` 类自带的胶囊体、骨骼网格体和 `CharacterMovementComponent`。
- 使用 `CharacterMovementComponent` 管理角色的行走、空中控制和跳跃，不再自行编写底层位移与碰撞逻辑。
- 配置角色朝移动方向转向，并设置旋转速度、最大行走速度、跳跃初速度和空中控制等基础参数。
- 关闭角色对 Controller Pitch、Yaw 和 Roll 的直接继承，使控制器旋转主要用于驱动第三人称摄像机。

### 2. 使用 Enhanced Input 配置输入资源

- 新建 `IA_Move`、`IA_Jump` 和 `IA_Look` 等 Input Action 资源，根据输入用途分别设置为 Axis2D 或 Boolean 类型。
- 新建 `IMC_Default` Input Mapping Context，将键盘、鼠标和手柄输入映射到对应的 Input Action，替代直接在代码中判断具体按键的硬编码方式。
- 在本地 PlayerController 初始化输入时获取 `EnhancedInputLocalPlayerSubsystem`，并通过 `AddMappingContext` 添加默认输入映射上下文。
- 在角色的 `SetupPlayerInputComponent` 中将普通 Input Component 转换为 `EnhancedInputComponent`，再绑定各个 Action 的触发事件。
- `IA_Move` 和 `IA_Look` 使用 `Triggered` 事件持续读取二维输入；`IA_Jump` 使用 `Started` 调用 `Jump`，并使用 `Completed` 调用 `StopJumping`。

主要输入资源配置如下：

| 资源 | Value Type | 主要映射 | 用途 |
| --- | --- | --- | --- |
| `IMC_Default` | Input Mapping Context | 键盘、鼠标与手柄 | 集中管理默认角色输入映射 |
| `IA_Move` | Axis2D | W/A/S/D、手柄左摇杆 | 输出前后与左右移动输入 |
| `IA_Jump` | Boolean | Space、手柄底部面键 | 控制开始和停止跳跃 |
| `IA_Look` | Axis2D | 鼠标移动、手柄右摇杆 | 输出摄像机水平与垂直旋转输入 |

### 3. 实现角色移动、跳跃与视角控制

- 在 `Move` 中读取 `FInputActionValue` 的二维向量，将 X、Y 分量分别作为左右和前后输入。
- 获取 Controller 的控制旋转，仅保留 Yaw 角，再分别计算摄像机朝向对应的前向量与右向量。
- 调用 `AddMovementInput` 叠加前后和左右输入，使角色移动方向始终以当前摄像机朝向为基准。
- 使用 `Character` 自带的 `Jump` 和 `StopJumping` 完成跳跃输入响应，并由 `CharacterMovementComponent` 处理离地、下落与落地过程。
- 在 `Look` 中调用 `AddControllerYawInput` 和 `AddControllerPitchInput`，实现鼠标或手柄控制摄像机水平、垂直转向。

### 4. 添加第三人称跟随摄像机

- 为角色添加 `SpringArmComponent` 作为 Camera Boom，并将其连接到角色根组件。
- 在 Spring Arm 末端添加 `CameraComponent`，形成角色、摇臂和摄像机的第三人称跟随结构。
- 让 Spring Arm 使用 Pawn Control Rotation，使摄像机能够围绕角色旋转，同时避免摄像机旋转直接带动角色原地转向。
- 开启 Camera Lag，使视角跟随更加平滑；开启 Do Collision Test，使 Spring Arm 遇到墙体等障碍物时能够自动缩短，减少摄像机穿墙现象。

### 5. 运行测试与录制演示

- 在测试关卡中分别验证 W/A/S/D 移动、跳跃、角色转向和摄像机环绕功能。
- 检查改变摄像机朝向后的移动结果，确认角色能够按照当前视角的前后左右方向移动。
- 靠近场景墙体测试 Spring Arm 碰撞收缩，确认摄像机不会直接穿过障碍物。
- 录制角色在测试关卡中连续移动、跳跃和转动摄像机的演示过程，作为本周可操控角色 Demo GIF。

## 实现结果

最终完成了一个具备基础移动、跳跃、朝移动方向转向和第三人称视角控制能力的可操控角色。输入逻辑已经由 `IA_Move`、`IA_Jump`、`IA_Look` 和 `IMC_Default` 等资源统一配置，键盘、鼠标与手柄可以复用同一套角色逻辑。Spring Arm 跟随、Camera Lag 与碰撞测试也能够正常工作，并完成了测试关卡中的操作演示录制。

## 本周总结

通过本周练习，我理解了 `Character` 与 `CharacterMovementComponent` 在角色移动、旋转和跳跃中的职责，也掌握了 Enhanced Input 将“输入含义”“按键映射”和“游戏逻辑”分离的配置方式。与硬编码按键相比，Input Action 和 Input Mapping Context 更便于扩展多种输入设备、调整键位以及切换不同操作上下文。Spring Arm 与 Camera Component 的组合则完善了第三人称角色的基础体验，为后续加入动画状态、冲刺、交互和战斗等功能做好了准备。
