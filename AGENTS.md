# Agent Guide for MargeletDesktop (Margy)

This guide defines repository-wide instructions and architectural knowledge for coding agents working with the MargeletDesktop codebase.

---

## 1. Project Overview & Architecture

**MargeletDesktop** (also known as Margy / MareletDesktop) is a custom fork of Telegram Desktop (tdesktop) with extensive personalization, plugin capabilities, UI enhancements, and desktop shell integration.

### Core Custom Module (`Telegram/SourceFiles/margy/`)
All client-specific enhancements are implemented in the `margy` module and wired via `Telegram/CMakeLists.txt` (`add_subdirectory(SourceFiles/margy)`):

- **`shell` (`margy/shell/`)**: Dynamic Quickshell/desktop shell theme synchronization (`--shell-color`, `--color`, `--amoled`, `~/.cache/shell_color`, IPC).
- **`badges` (`margy/badges/`)**: Profile and user badges, 3D badge viewer (`margy_plane_3d`), badge gallery, custom icons, Yoxi badge, Kent badge.
- **`cats` (`margy/cats/`)**: Animated cats overlay and manager (`margy_cats_manager`, `margy_cats_box`).
- **`plugins` (`margy/plugins/`)**: Python & Luau plugin runtime (`margy_plugin_host`, `margy_plugin_manager`, `margy_host.py`, `margy_api.d.luau`), plugin settings UI, console box, hooks, typing overlay, snow overlay.
- **`icons` (`margy/icons/`)**: Custom icon packs engine (Default, Lucide, Tabler, Phosphor).
- **`settings` (`margy/settings/`)**: Dedicated Margy settings section in Telegram settings.
- **`sound` (`margy/sound/`)**: Custom sounds (meow notification sound, embedded audio).
- **`tags` (`margy/tags/`)**: Audio metadata/tags editor dialog.
- **`wall` (`margy/wall/`)**: Custom user profile wall.
- **`gradient` (`margy/gradient/`)**: Custom avatar/profile header gradient styling.
- **`fonts` (`margy/fonts/`)**: Custom font and emoji font selection.
- **`gifts` (`margy/gifts/`)**: Unhiding gifts and custom gift visuals.
- **`markup` (`margy/markup/`)**: Formatting tags (`margy_dim`, `margy_outline`, `margy_rainbow`, `margy_size`).
- **`streamer` (`margy/streamer/`)**: Streamer privacy mode (hiding usernames/sensitive data).
- **`seizure` (`margy/seizure/`)**: Visual effect modes.
- **`proxy` (`margy/proxy/`)**: Custom proxy routing and managers.

---

## 2. Shell Theme Integration (Quickshell Sync)

MargeletDesktop supports instant accent color and AMOLED theme syncing from external Linux shells (like Quickshell) without restarting the client and without opening duplicate windows.

### CLI Contract
| Flag | Description |
|---|---|
| `--shell-color` | Signal flag indicating the execution is triggered by a shell theme sync script. |
| `--color <#HEX>` | Accent color in `#RRGGBB` format (e.g. `#c8b36a` or `#8a9a73`). |
| `--amoled` | Flag enabling pure black (`#000000`) AMOLED backgrounds for panels, dialogs, and headers. |

### Single-Instance IPC
- Handled in `Core::Sandbox` via `QLocalSocket` / `QLocalServer`.
- When `--shell-color` is passed:
  - If Telegram is already running, the secondary instance sends `CMD:shell_theme <HEX> <AMOLED:0|1>;` over the local socket.
  - The primary instance executes `Margy::ShellTheme::HandleIpcCommand` and returns `windowId = 0`.
  - The secondary instance quits immediately (`code 0`) without focusing or creating any GUI window.
- If Telegram is launched fresh, the CLI arguments are applied on initial startup in `Application::startSettingsAndBackground()`.

### Automatic FileWatcher
- In addition to CLI calls, `Margy::ShellTheme::Watcher` watches `~/.cache/shell_color` (and `~/.cache/shell_amoled`).
- Any update made to `~/.cache/shell_color` triggers an automatic, on-the-fly theme update across all active Qt widgets.

---

## 3. Submodule Patches (`Telegram/patches/`)

Submodule modifications (such as `Telegram/lib_ui`) are tracked via `.patch` files applied during build.

- **`Telegram/patches/lib_ui.patch`**: Patches `Telegram/lib_ui` to support icon mask override hooks, Material Design 3 toggle switches, and custom styling.
- **CRITICAL LINKER GOTCHA**:
  - Never place hook functions or exported symbols (such as `SetIconMaskOverrideHook`) inside an anonymous `namespace { ... }` in `.patch` files.
  - Doing so gives the function internal linkage, which causes the linker (`ld.lld`) to fail with `undefined symbol: style::internal::SetIconMaskOverrideHook(...)`.
  - Exported hook functions must be declared and defined within `namespace style::internal`.

---

## 4. Build System & CI

### Linux Docker Build
- The build runs in a CentOS Docker container: `Telegram/build/docker/centos_env/build.sh` (or `build_debug.sh`).
- Container image: `ghcr.io/telegramdesktop/tdesktop/centos_env:latest` (tagged as `tdesktop:centos_env`).

### CMake Quirks
- The build runs through `cmake_helpers/run_cmake.py`, which injects:
  - `--warn-uninitialized`
  - `-Werror=dev`
- **Never reference uninitialized variables in `CMakeLists.txt`** (e.g., using `${qt_loc}` when `qt_loc` is not set will immediately abort CMake configuration with `-Werror=dev`).

### Release vs Debug Builds & Binary Size
- **Debug builds (`CONFIG=Debug`)**: Executable is ~1.8 GB due to massive unstripped DWARF debug symbols.
- **Release builds (`CONFIG=Release`)**: Highly optimized (`-O3`), and running `strip --strip-unneeded artifact/*` brings the final standalone binary down to **~45–60 MB**.

### GitHub Actions Workflows (`.github/workflows/`)
- `linux.yml`: Linux x64 build inside Docker, uses ccache, validates generated binary, strips symbols, and uploads `MareletDesktop-Linux-x64` artifact.
- `win.yml`: Windows MSVC x64 build.

---

## 5. Development Guidelines & Code Style

### Comments Are Rationed
- A comment is one line; two or three only when the block opens with `// WHY:`.
- Say **why**, never **what**.
- Inline comments that label positional arguments for TL/MTP schemas or generated APIs are permitted.

### Coding Conventions
- **Use `_q` for QString literals**: Always write `u"text"_q` instead of `QStringLiteral("text")`.
- **Type deduction**: Prefer `auto` / `const auto &` where type is deduced.
- **Never discard results with casts**: `static_cast<void>(...)` and `(void)expr` are banned.
- **Platform checks**:
  - **Never use `Q_OS_LINUX` in new code.**
  - Telegram Desktop uses a 3-way platform model: Windows, macOS, and All-Other (Linux, BSD, etc.).
  - For All-Other, always write:
    ```cpp
    #if !defined Q_OS_WIN && !defined Q_OS_MAC
    // All-other platform code
    #endif
    ```
    or at runtime: `Platform::IsLinux()`.
- **CMake Platform Split**:
  ```cmake
  if (WIN32)
      set(platform_source platform/win.cpp)
  elseif (APPLE)
      set(platform_source platform/mac.mm)
  else()
      set(platform_source platform/linux.cpp)
  endif()
  ```

### Local Storage Binary Serialization (`QDataStream`)
`Core::Settings` and `Main::SessionSettings` use sequential binary serialization.
- **New fields must ALWAYS be appended at the end** of the stream, never in the middle.
- Reading new fields must be guarded with `!stream.atEnd()` and provide fallbacks.
- Inserting fields in the middle corrupts saved user profiles and session data across updates.

### UI Styling & Themes
- Never hardcode dimensions, margins, paddings, or coordinates in C++ (`px` values scale with interface DPI).
- Define values in `.style` files and reference them as `st::propertyName`.
- Colors: To update colors in runtime, modify `style::main_palette`, invoke `style::internal::ResetIcons()`, `style::NotifyPaletteChanged()`, and trigger `Window::Theme::Background()->appliedEditedPalette()`.
