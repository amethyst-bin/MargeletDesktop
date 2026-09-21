#pragma once

#include <QColor>
#include <QString>

namespace Margy::Seizure {

[[nodiscard]] bool IsEnabled();
void SetEnabled(bool enabled);

[[nodiscard]] QColor CurrentColor();
[[nodiscard]] QString CurrentColorCss();

} // namespace Margy::Seizure
