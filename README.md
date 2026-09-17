# StudyPet

StudyPet is a Windows desktop learning companion built with C++17, Qt 6
Widgets and CMake. It combines a transparent, draggable desktop pet with
PNG-frame animation, Pomodoro study tools, reminders, local persistence and
an optional AI chat module.

## Current features

- Transparent always-on-top desktop pet with tray controls, dragging, edge-aware
  walking, click feedback and optional click-through.
- Qt-timer-driven state and animation systems; no busy-loop rendering.
- Settings, Pomodoro timer, reminders, to-do list, study statistics and pet
  attributes saved atomically under `QStandardPaths::AppDataLocation`.
- Optional OpenAI-compatible chat. Without a key it automatically uses the
  offline mock client, so course demonstrations work without a network.
- Four-frame Hello Kitty visual sequences for idle/blink, walk, talking, happy,
  reminder and sleep. See [the animation preview](docs/animation-assets.md).

## Build on Windows

Install Qt 6 for **MSVC 2022 x64** and CMake 3.21 or newer, then configure the
matching Qt prefix (adjust the version to the installed kit):

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DBUILD_TESTING=ON -DCMAKE_PREFIX_PATH="C:/Qt/6.11.2/msvc2022_64"
cmake --build build --config Debug --parallel
ctest --test-dir build -C Debug --output-on-failure
.\build\Debug\StudyPet.exe
```

The GitHub Actions workflow also configures a Windows MSVC build and runs all
CTest targets on pushes to `feat/**`, pull requests, and manual dispatch.

## Animation assets and fallback

`resources/config/animations.json` describes each action using its frame list,
interval, looping mode and optional mirroring. The new active frames live in
`resources/animations/kitty/`; the earlier blue placeholders remain embedded
as `fallbackFrames`. A missing or malformed primary image therefore produces a
warning and safely uses the matching placeholder animation.

`WalkLeft` mirrors the right-facing walk frames in `AnimationPlayer`, while
all other actions load their declared PNG sequence directly. The settings
window's animation-speed control updates the current frame timer immediately.

## Optional AI configuration

Set `STUDYPET_API_KEY` in the environment, or create the ignored local file
`config/private.json` from `config/private.example.json`. Base URL, model,
timeout and retained context count are configured in Settings. Never commit a
real key. See [docs/phase-4-ai-chat.md](docs/phase-4-ai-chat.md) for the
request flow and the offline-demo behavior.

## Copyright notice

Hello Kitty is a Sanrio character. The reference-based desktop-pet frames in
this repository are provided solely for non-commercial course learning and
demonstration. StudyPet does not claim ownership of the character, artwork or
related trademarks.

## Project layout

```text
src/animation/  JSON-driven PNG animation player
src/core/       controller, state machine and settings
src/service/    persistence, study, reminder and AI services
src/window/     pet, study, settings and chat windows
resources/      packaged PNG assets and animation configuration
tests/          Qt Test / CTest coverage
docs/           architecture, requirements and asset documentation
```

