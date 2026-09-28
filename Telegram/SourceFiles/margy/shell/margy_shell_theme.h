#pragma once

#include <QtGui/QColor>
#include <QtCore/QString>
#include <optional>

namespace Margy::ShellTheme {

void SetCommandLineArgs(bool shellColor, const QString &colorHex, bool amoled);

[[nodiscard]] bool HasShellColorCommand();

[[nodiscard]] QString IpcCommandString();

void HandleIpcCommand(const QString &commandArgs);

void Start();

void Apply(const QColor &accentColor, bool amoled);

[[nodiscard]] std::optional<QColor> ReadCacheColor();

[[nodiscard]] bool CheckAmoled();

} // namespace Margy::ShellTheme
