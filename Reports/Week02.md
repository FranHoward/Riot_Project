# Week 02：Blueprint 复用、C++ 与版本管理周报

## 本周目标

本周主要学习 Blueprint Interface 等逻辑复用手段，理解 C++ 属性和函数暴露给 Blueprint 的机制，并为 Unreal Engine 项目搭建适合二进制资产的 Git 版本管理流程。

## 本周完成内容

### 1. 使用 Blueprint Interface 复用拾取逻辑

- 新建 `BPI_Interactable` Blueprint Interface，并定义 `OnInteract` 交互函数。
- 创建 `BP_Actor_1` 和 `BP_Actor_2` 两种不同的 Actor Blueprint，并分别实现该接口。
- 编写基于重叠检测的测试逻辑，通过接口调用触发拾取行为，没有直接转换为具体的 Actor 类型。
- 在两个 Actor 中复用统一的交互入口，并根据各自配置执行不同的拾取效果，验证接口多态能够正常生效。

### 2. 将 C++ 功能暴露给 Blueprint

- 新建继承自 `AActor` 的 C++ 基类 `APickupBase`，并使用 `UCLASS(Blueprintable)` 允许 Blueprint 继承该类。
- 声明 `PickupValue` 字段，并通过 `UPROPERTY(EditAnywhere, BlueprintReadWrite)` 将其暴露给 Blueprint。
- 声明 `Collect` 函数，并通过 `UFUNCTION(BlueprintCallable)` 允许 Blueprint 调用该函数。
- 在 `Collect` 函数中输出拾取物的数值信息，用于验证 Blueprint 对 C++ 函数的调用结果。
- 编译项目后创建 `BP_PickupBase` Blueprint 子类，确认 `PickupValue` 能够在 Details 面板中修改，并可通过不同数值完成差异化配置。

### 3. 配置 Unreal Engine 项目的版本管理流程

- 参考 Unreal Engine 项目常用的 `.gitignore` 模板，配置 `Binaries/`、`Intermediate/`、`Saved/` 和 `DerivedDataCache/` 等生成目录的忽略规则。
- 补充插件构建目录、编辑器缓存、生成工程文件和编译产物等忽略规则，避免无关文件进入版本库。
- 安装并在当前仓库初始化 Git LFS，同时安装对应的推送钩子。
- 在 `.gitattributes` 中配置 `*.uasset`、`*.umap`、`*.ubulk`、`*.uexp` 和 `*.uptnl` 等 Unreal Engine 二进制资产的 LFS 追踪规则。
- 使用 `git lfs track` 和 `git lfs status` 检查配置，确认二进制资产能够被 Git LFS 正确识别。

## 实现结果

最终完成了 Blueprint Interface 拾取交互、C++ 拾取物基类及 Blueprint 子类的基础实现。两个不同的 Actor 能够通过统一接口响应交互，Blueprint 子类也能够编辑 C++ 暴露的属性并调用其函数。同时，项目已具备 Unreal Engine 专用的文件忽略规则和 Git LFS 追踪配置，可通过 `git lfs status` 查看当前二进制资产的追踪状态。

## 本周总结

通过本周练习，我进一步理解了接口、继承和多态在 Unreal Engine 项目中的应用。Blueprint Interface 降低了交互逻辑对具体 Actor 类型的依赖，而 C++ 与 Blueprint 的结合兼顾了底层逻辑复用和可视化配置效率。Git LFS 与 `.gitignore` 的配置也完善了项目的协作基础，为后续持续添加关卡和资产、进行团队版本管理做好了准备。
