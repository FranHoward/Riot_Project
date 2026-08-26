# Unity 与 Unreal Engine 概念对照表

下表对照 Unity 中常见的开发概念与 Unreal Engine（UE）中的对应实现。两者的功能通常相近，但并非完全等价。

| 序号 | Unity / 通用概念 | Unreal Engine 对应概念 | 相同点 | 关键区别 | 典型用法 |
| --- | --- | --- | --- | --- | --- |
| 1 | 场景物体（GameObject） | Actor | 都可以放置在场景或关卡中，并通过添加组件获得模型、碰撞、音频等能力。 | Unity 的 GameObject 本身主要是组件容器；UE 的 Actor 是可放入关卡或动态生成的对象类型，通常以 `SceneComponent` 作为根组件，并可拥有多个其他组件。并非所有 UE 对象都是 Actor。 | 创建门、敌人、道具或触发器等关卡对象时，通常建立 Actor 或 Actor 的 Blueprint 子类。 |
| 2 | 可复用预制体（Prefab） | Blueprint Class | 都能把组件、默认属性和行为封装成模板，并从模板创建多个实例。 | Prefab 更接近保存好的对象模板；Blueprint Class 是一个完整的类，可以包含继承关系、变量、函数、组件和事件图。 | 创建 `BP_Door`，配置门框、门板和开门逻辑，再将多个实例拖入关卡，或在运行时生成实例。 |
| 3 | 行为脚本（MonoBehaviour） | Actor 自身事件图或 ActorComponent | 都能为场景对象增加可编程行为，并响应初始化、碰撞、输入等事件。 | UE 中仅服务于单个对象的逻辑可直接写在 Actor 的 Blueprint 事件图中；需要被多种 Actor 复用的独立功能更适合封装成 `ActorComponent`。C++ 中也可以通过继承 Actor 或 ActorComponent 实现。 | 门的开关逻辑可写在 `BP_Door` 自身；生命值、交互或背包等可复用功能可制作成 ActorComponent。 |
| 4 | 初始化时机（Awake / Start） | Construction Script / BeginPlay | 都用于在对象开始工作前设置初始状态。 | `Construction Script` 会在编辑器中放置或修改 Actor、以及运行时生成 Actor 时执行，适合根据属性构建组件和外观；`BeginPlay` 在游戏开始或 Actor 进入游戏时执行一次，适合运行时初始化。Construction Script 可能在编辑器中反复执行，不应放置只允许执行一次的游戏逻辑。 | 根据“墙段数量”在 Construction Script 中生成预览；在 BeginPlay 中获取玩家引用、设置计时器或初始化运行时状态。 |
| 5 | 每帧更新（Update） | Tick | 都会在游戏运行期间按帧调用，并提供处理持续逻辑的机会。 | Actor 或组件必须允许 Tick 才会执行；Tick 会带来持续性能成本。UE 的 Tick 通常接收 `Delta Seconds`，用于实现与帧率无关的运动。能用事件、计时器或时间轴完成的逻辑通常不必放在 Tick 中。 | 在 Tick 中用 `Delta Seconds` 更新需要持续追踪的目标；低频检查可改用 Timer，动画变化可使用 Timeline。 |
| 6 | 实例化 / 销毁（Instantiate / Destroy） | SpawnActor / DestroyActor | 都用于在运行时创建和移除场景中的对象实例。 | UE 通过 Blueprint 的 `Spawn Actor from Class` 节点或 C++ 的 `GetWorld()->SpawnActor<T>()` 创建 Actor；通过 `DestroyActor` 节点或 Actor 的 `Destroy()` 请求销毁。Actor 的销毁通常会延迟到安全时机完成。组件的创建和移除使用另一套组件接口。 | 发射子弹时 SpawnActor，敌人死亡或临时特效结束时 DestroyActor。生成后如需保存引用，应先判断引用是否有效。 |
| 7 | 面板可编辑字段（Inspector 字段） | `UPROPERTY(EditAnywhere)` | 都能把变量暴露到编辑器面板，让设计人员为类或关卡实例调整数值。 | C++ 变量必须使用 `UPROPERTY` 宏才能被 UE 的反射和编辑器系统识别。`EditAnywhere` 允许在类默认值和实例中编辑；它不等于允许 Blueprint 读写，若还需要脚本访问，可增加 `BlueprintReadWrite` 或 `BlueprintReadOnly`。纯 Blueprint 变量则可勾选 **Instance Editable**。 | `UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Door") float OpenAngle = 90.0f;` 可让门的开启角度在 Details 面板中编辑。 |
| 8 | 预制体变体（Prefab Variant） | Blueprint 子类 | 都能继承一个基础模板，并在保留父级结构和逻辑的同时修改部分配置。 | Blueprint 子类属于类继承：它会继承父类变量、组件、函数和事件，并可修改默认值或重写允许重写的逻辑。父类更新通常会传播到子类，但子类已经覆盖的默认值会保留自己的设置。 | 以 `BP_EnemyBase` 为父类创建 `BP_EnemyMelee` 和 `BP_EnemyRanged`，分别调整模型、生命值和攻击方式。 |

## 总结

Unity 的工作方式通常围绕 **GameObject、Component 和 Prefab** 展开；UE 则主要围绕 **Actor、ActorComponent、Blueprint Class 和类继承** 展开。迁移概念时应理解各自的生命周期和类型体系，不要只按名称进行一一替换。
