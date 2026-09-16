# StudyPet

一个面向大学生的 Windows 学习陪伴桌宠。第一版使用 **C++17、Qt 6 Widgets、CMake 与 PNG 透明帧**，不依赖 Live2D 或 Python。

## 当前进度

阶段 3 已加入设置窗口、JSON 本地保存、番茄钟、喝水/久坐提醒、学习统计与养成属性。阶段 1/2 的桌宠交互和动画继续保留。

截图将放在 `docs/images/`（后续阶段补充）。

## 技术栈

- C++17
- Qt 6.4+（Widgets）
- CMake 3.21+
- MSVC 2022（Windows 10/11）

## 构建与运行

先安装 Qt 6 的 **MSVC 2022 64-bit** 组件，并让 CMake 能找到 Qt：可设置 `CMAKE_PREFIX_PATH` 为 Qt 安装目录，例如 `C:/Qt/6.8.0/msvc2022_64`。

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:/Qt/6.8.0/msvc2022_64"
cmake --build build --config Debug
.\build\Debug\StudyPet.exe
```

Release 构建：

```powershell
cmake --build build --config Release
```

窗口关闭时会隐藏到托盘；请在托盘菜单选择“退出 StudyPet”以结束程序。

## 项目结构

```text
src/core/       应用控制器与领域逻辑
src/animation/  JSON 配置驱动的 PNG 序列帧播放器
src/window/     Qt 窗口与桌面交互
resources/      原创占位动画和动画配置
docs/           软件工程文档（后续阶段持续补全）
tests/          Qt Test/CTest 单元测试
```

## 动画配置

`resources/config/animations.json` 的每个动作以 JSON 描述帧资源、`intervalMs` 与 `loop`。`AnimationPlayer` 只接受存在且可读取的帧；错误配置或缺失资源会记录警告并跳过，程序不会崩溃。当前动作包括 `Idle`、`WalkLeft`、`WalkRight`、`Sleep`、`Dragged`、`Clicked` 和 `Talking`。

阶段 2 的全部 PNG 都是同一原创占位角色的帧占位副本；后续可替换为原创动作帧，而无需改动窗口或状态机逻辑。

## 测试与 CI

配置时保持 `BUILD_TESTING=ON`（默认值），然后执行：

```powershell
ctest --test-dir build -C Debug --output-on-failure
```

测试覆盖状态机的合法/非法转换、动画 JSON 解析，以及缺失 PNG 资源的安全失败。GitHub Actions 会在 Windows 上安装 Qt 6、配置工程、构建 Debug 并运行 CTest。

## 本地数据

用户数据通过 `QStandardPaths::AppDataLocation/study-pet.json` 保存，不写入源码目录。文件使用 `schemaVersion: 1` 和 `QSaveFile` 原子写入，损坏、缺失或类型不正确时回退到安全默认值。

## 配置与 AI

第 4 阶段将支持 OpenAI 兼容接口。密钥仅从环境变量或 `config/private.json` 读取；该文件已被忽略，绝不能提交真实密钥。未配置 AI 时基础功能仍可正常运行。

## 后续规划

1. PNG 序列帧播放器、独立行为状态机与单元测试。
2. 设置、JSON 本地存储、番茄钟、提醒、待办和养成数据。
3. 可关闭的 AI 聊天、流式输出与离线演示模式。
4. Live2D 作为可选的第二阶段渲染器，不提交 SDK 或未经授权素材。

