#pragma once

#include <QString>

namespace Margy::Streamer {

[[nodiscard]] QString MaskPhone(const QString &phone);
[[nodiscard]] bool ShouldHideUsername(bool isSelf);
[[nodiscard]] QString MaskUsername(const QString &username, bool isSelf);

} // namespace Margy::Streamer
