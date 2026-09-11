# Week 05：角色动画状态机与摄像机优化周报

## 本周目标

本周主要为第三人称角色赋予基础动作表现，掌握 Animation Blueprint、状态机和 Blend Space 的搭建方法，使角色能够根据移动速度与空中状态自然切换 Idle、Walk、Run、Jump 和 Fall 动画。同时继续优化 Spring Arm 的碰撞与跟随参数，改善角色移动和贴墙时的摄像机体验。

## 本周完成内容

### 1. 获取并导入免费角色动画资源

- 从 Mixamo 下载 Idle、Walk、Run 和 Jump 等免费动画资源，并选择与当前角色骨骼结构接近的模型作为动画来源。
- 将动画文件导入 Unreal Engine，检查 Skeleton、帧率、播放循环和根骨骼设置，确保动画资源能够在编辑器中正常预览。
- 为源角色和目标角色配置 IK Rig，在 Retargeter 中建立 Root、Spine、Head、Arm 和 Leg 等骨骼链，并检查骨盆与四肢的映射关系。
- 调整 Retarget Pose，修正重定向后手臂姿势异常、骨盆高度不一致等问题，再将动画导出到项目角色所使用的 Skeleton。
- 对 Walk 和 Run 动画进行原地化处理，锁定水平位移并关闭不需要的 Root Motion，避免播放动画时角色模型脱离胶囊体或持续移动到屏幕边缘。

### 2. 搭建 Animation Blueprint 状态机

- 新建角色 Animation Blueprint，并确认其 Target Skeleton 与角色 Skeletal Mesh 使用的 Skeleton 一致。
- 在 AnimGraph 中创建 Locomotion State Machine，组织 Idle、Walk、Run、Jump 和 Fall 等基础动作状态。
- 使用 `Speed` 判断角色处于静止、行走或奔跑状态，使用 `IsFalling` 判断角色是否已经离开地面。
- 为移动状态设置进入和退出阈值，使 Walk 与 Run 使用不同的切换速度，减少速度在临界值附近变化时反复跳转的问题。
- 在空中状态中结合 `IsFalling` 与角色的垂直速度区分起跳和下落，并在角色落地后返回 Idle 或移动状态。

主要状态切换条件如下：

| 状态切换 | 主要条件 | 作用 |
| --- | --- | --- |
| Idle → Walk | `Speed > 10` 且未处于空中 | 角色开始移动时播放行走动画 |
| Walk → Idle | `Speed <= 10` | 角色停止移动时回到待机动画 |
| Walk → Run | `Speed > 300` | 移动速度达到奔跑范围时切换为跑步 |
| Run → Walk | `Speed <= 250` | 速度降低后平滑回到行走状态 |
| Ground → Jump/Fall | `IsFalling = true` | 离地后进入空中动画 |
| Jump/Fall → Ground | `IsFalling = false` | 落地后返回地面移动状态 |

### 3. 使用 Blend Space 混合 Walk 与 Run

- 新建一维 Blend Space，并将 `Speed` 设置为横轴输入参数。
- 在低速区域放置 Idle 和 Walk 动画，在高速区域放置 Run 动画，使动作能够随实际移动速度连续混合。
- 根据角色的移动速度范围调整样本位置，并设置适当的插值时间，避免按下或松开移动键时动画突然跳变。
- 使用原地化后的 Walk 和 Run 动画作为 Blend Space 样本，使角色位移完全由 Character Movement 控制，动画只负责视觉表现。
- 运行测试后确认普通 W 输入对应行走速度，按住 Shift 与 W 时切换到奔跑速度，并能够在行走和奔跑动画之间自然过渡。

### 4. 在 Blueprint Update Animation 中更新状态变量

- 在 Animation Blueprint 的 `Blueprint Update Animation` 中获取当前动画实例所属的 Pawn，并转换为项目角色类型。
- 读取角色 Velocity，忽略 Z 轴后计算水平向量长度，并将结果写入 `Speed`。
- 从 Character Movement 获取 `Is Falling`，并将结果写入 `IsFalling`。
- 在角色 Blueprint 的 Skeletal Mesh 上指定当前 Animation Blueprint，确保游戏运行时使用的是本周创建的动画逻辑。
- 通过运行时调试检查变量变化，确认静止、行走、奔跑、起跳和下落时的 `Speed` 与 `IsFalling` 均能正确驱动状态机。

### 5. 优化 Spring Arm 的碰撞与跟随手感

- 开启 Spring Arm 的 Camera Lag，使摄像机跟随角色时保持平滑，同时避免延迟过大造成操作拖沓。
- 调整 Camera Lag Speed 与 Camera Lag Max Distance，平衡移动过程中的稳定性和响应速度。
- 开启 Do Collision Test，并调整 Probe Size 和 Probe Channel，使摄像机接近墙体时能够及时缩短摇臂距离。
- 分别测试空旷区域移动、快速转向以及角色贴墙等场景，检查摄像机是否出现抖动、突然拉近或穿过墙体的问题。
- 经过参数调整后，Spring Arm 能够在接近障碍物时自动收缩，并在离开墙体后平滑恢复正常距离。

## 实现结果

最终完成了角色基础动画资源的导入、骨骼重定向、原地化处理以及 Animation Blueprint 状态机搭建。角色能够根据 `Speed` 和 `IsFalling` 在 Idle、Walk、Run、Jump 与 Fall 动作之间切换；Walk 和 Run 通过 Blend Space 进行混合，普通移动与 Shift 奔跑时的动画过渡较为自然。动画不再产生额外的水平位移，角色位置由 Character Movement 统一控制。Spring Arm 的跟随和碰撞参数也完成了调整，角色贴近墙体时摄像机能够自动收缩，未出现明显的镜头穿模。

## 本周总结

通过本周练习，我掌握了从外部获取动画资源、配置 IK Rig 与 Retargeter、修正重定向姿势并将动画应用到项目角色的基本流程。同时，我理解了 Animation Blueprint 中 Event Graph、AnimGraph、状态机和 Blend Space 之间的分工：Event Graph 负责持续更新角色状态变量，状态机负责选择当前动作，Blend Space 则负责在不同移动速度对应的动画之间进行平滑混合。对原地动画和 Spring Arm 参数的进一步处理，也让我认识到角色动画、实际移动和摄像机反馈需要相互配合，才能形成自然稳定的第三人称操作体验。
